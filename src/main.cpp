#include "lvgl.h"
#include <Arduino.h>
#include <math.h>

// Variables globales pour le radar
static int valeurs[10] = {0}; // Tableau pour stocker les 10 dernières valeurs
static int current_read_index = 0;        // Index pour le tableau circulaire
static long somme = 0;       // Somme des valeurs dans le tableau

// Variables pour les rectangles
static lv_obj_t * green_rect_1 = NULL;
static lv_obj_t * green_rect_2 = NULL;
static lv_obj_t * yellow_rect_1 = NULL;
static lv_obj_t * yellow_rect_2 = NULL;
static lv_obj_t * red_rect_1 = NULL;
static lv_obj_t * red_rect_2 = NULL;

static void event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_CLICKED) {
        LV_LOG_USER("Clicked");
    }
    else if(code == LV_EVENT_VALUE_CHANGED) {
        LV_LOG_USER("Toggled");
    }
}

void testLvgl()
{
  // Initialisations générales
  // Note : Les boutons ont été commentés précédemment.
  // La déclaration 'lv_obj_t * label;' est supprimée car elle n'est plus utilisée.

  // Création des rectangles pour le radar (deux blocs par couleur, occupant toute la hauteur de l'écran)

  // Calcule la hauteur de chaque rectangle individuel pour remplir l'écran (hauteur écran / 6 blocs = 40px par bloc pour un écran 240px de haut)
  lv_coord_t rect_height = lv_display_get_vertical_resolution(NULL) / 6;
  lv_coord_t full_width = lv_display_get_horizontal_resolution(NULL);

  // Zone Verte (le plus éloigné)
  // Bloc Vert Supérieur
  green_rect_1 = lv_obj_create(lv_screen_active());
  lv_obj_set_size(green_rect_1, 250, rect_height);
  lv_obj_align(green_rect_1, LV_ALIGN_TOP_MID, 0, 0 * rect_height); // Collé au sommet
  lv_obj_set_style_bg_color(green_rect_1, lv_color_make(0x00, 0xFF, 0x00), LV_PART_MAIN); // Vert
  lv_obj_set_style_bg_opa(green_rect_1, LV_OPA_COVER, LV_PART_MAIN);

  // Bloc Vert Inférieur (collé au précédent)
  green_rect_2 = lv_obj_create(lv_screen_active());
  lv_obj_set_size(green_rect_2, 250, rect_height);
  lv_obj_align(green_rect_2, LV_ALIGN_TOP_MID, 0, 1 * rect_height); // Collé juste en dessous
  lv_obj_set_style_bg_color(green_rect_2, lv_color_make(0x00, 0xFF, 0x00), LV_PART_MAIN); // Vert
  lv_obj_set_style_bg_opa(green_rect_2, LV_OPA_COVER, LV_PART_MAIN);

  // Label pour la zone Verte (centré sur la zone combinée des deux blocs verts)
  lv_obj_t * green_label = lv_label_create(lv_screen_active());
  lv_label_set_text(green_label, "Zone Loin (>50 cm)");
  // Centré verticalement sur la zone combinée (0*rect_height + 1*rect_height / 2)
  lv_obj_align(green_label, LV_ALIGN_TOP_MID, 0, (0 * rect_height) + (rect_height));


  // Zone Jaune (milieu)
  // Bloc Jaune Supérieur
  yellow_rect_1 = lv_obj_create(lv_screen_active());
  lv_obj_set_size(yellow_rect_1, 200, rect_height);
  lv_obj_align(yellow_rect_1, LV_ALIGN_TOP_MID, 0, 2 * rect_height); // Collé à la zone verte
  lv_obj_set_style_bg_color(yellow_rect_1, lv_color_make(0xFF, 0xFF, 0x00), LV_PART_MAIN); // Jaune
  lv_obj_set_style_bg_opa(yellow_rect_1, LV_OPA_COVER, LV_PART_MAIN);

  // Bloc Jaune Inférieur (collé au précédent)
  yellow_rect_2 = lv_obj_create(lv_screen_active());
  lv_obj_set_size(yellow_rect_2, 200, rect_height);
  lv_obj_align(yellow_rect_2, LV_ALIGN_TOP_MID, 0, 3 * rect_height); // Collé juste en dessous
  lv_obj_set_style_bg_color(yellow_rect_2, lv_color_make(0xFF, 0xFF, 0x00), LV_PART_MAIN); // Jaune
  lv_obj_set_style_bg_opa(yellow_rect_2, LV_OPA_COVER, LV_PART_MAIN);

  // Label pour la zone Jaune (centré sur la zone combinée des deux blocs jaunes)
  lv_obj_t * yellow_label = lv_label_create(lv_screen_active());
  lv_label_set_text(yellow_label, "Zone Moyenne (>30 cm)");
  // Centré verticalement sur la zone combinée (2*rect_height + 1*rect_height / 2)
  lv_obj_align(yellow_label, LV_ALIGN_TOP_MID, 0, (2 * rect_height) + (rect_height));


  // Zone Rouge (le plus proche)
  // Bloc Rouge Supérieur
  red_rect_1 = lv_obj_create(lv_screen_active());
  lv_obj_set_size(red_rect_1, 150, rect_height);
  lv_obj_align(red_rect_1, LV_ALIGN_TOP_MID, 0, 4 * rect_height); // Collé à la zone jaune
  lv_obj_set_style_bg_color(red_rect_1, lv_color_make(0xFF, 0x00, 0x00), LV_PART_MAIN); // Rouge
  lv_obj_set_style_bg_opa(red_rect_1, LV_OPA_COVER, LV_PART_MAIN);

  // Bloc Rouge Inférieur (collé au précédent)
  red_rect_2 = lv_obj_create(lv_screen_active());
  lv_obj_set_size(red_rect_2, 150, rect_height);
  lv_obj_align(red_rect_2, LV_ALIGN_TOP_MID, 0, 5 * rect_height); // Collé juste en dessous
  lv_obj_set_style_bg_color(red_rect_2, lv_color_make(0xFF, 0x00, 0x00), LV_PART_MAIN); // Rouge
  lv_obj_set_style_bg_opa(red_rect_2, LV_OPA_COVER, LV_PART_MAIN);

  // Label pour la zone Rouge (centré sur la zone combinée des deux blocs rouges)
  lv_obj_t * red_label = lv_label_create(lv_screen_active());
  lv_label_set_text(red_label, "Zone Proche (>10 cm)");
  lv_obj_align(red_label, LV_ALIGN_TOP_MID, 0, (4 * rect_height) + (rect_height)); // Centré sur la première moitié de la zone rouge
}



#include "lvglDrivers.h"

// à décommenter pour tester la démo
// #include "demos/lv_demos.h"

void mySetup()
{
  // à décommenter pour tester la démo
  // lv_demo_widgets();

  // Initialisations générales
  testLvgl();
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n=== DÉMARRAGE DU PROGRAMME ===");
  Serial.println("Capteur Sharp connecté sur la broche A0");
  Serial.println("--------------------------------");
}

void loop()
{
  // Inactif (pour mise en veille du processeur)
}

void myTask(void *pvParameters)
{
  TickType_t xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();

  bool toggle_color = false;      // Pour alterner  écran blanc / rouge
  TickType_t last_toggle_time = 0;
  const TickType_t toggle_interval = pdMS_TO_TICKS(500); // 500 ms

  while (1)
  { // reception valeur analogique dans la variable valeur
    int valeur = analogRead(A0);

    // matrice pour stocker 10 mesures et en faire la moyenne pour améliorer la précision
    somme = somme - valeurs[current_read_index];
    valeurs[current_read_index] = valeur;
    somme = somme + valeur;
    current_read_index = (current_read_index + 1) % 10;

    //calcule moyenne et en utilisant celleci calcul de la distance 
    float moyenne = somme / 10.0;
    float distance = 40797 * pow(moyenne, -1.284);
    if (distance < 0) distance = 0;
    if (distance > 80) distance = 80;

    TickType_t current_time = xTaskGetTickCount();

    if (distance < 10)
    {
      // Toutes les 500 ms, on inverse la couleur
      if ((current_time - last_toggle_time) >= toggle_interval)
      {
        toggle_color = !toggle_color;
        last_toggle_time = current_time;
      }

      if (toggle_color)
        lv_obj_set_style_bg_color(lv_screen_active(), lv_color_make(0xFF, 0x00, 0x00), LV_PART_MAIN); // Rouge
      else
        lv_obj_set_style_bg_color(lv_screen_active(), lv_color_make(0xFF, 0xFF, 0xFF), LV_PART_MAIN); // Blanc

      // Masquer tous les rectangles dans cette zone critique
      lv_obj_set_style_opa(green_rect_1, LV_OPA_0, LV_PART_MAIN);
      lv_obj_set_style_opa(green_rect_2, LV_OPA_0, LV_PART_MAIN);
      lv_obj_set_style_opa(yellow_rect_1, LV_OPA_0, LV_PART_MAIN);
      lv_obj_set_style_opa(yellow_rect_2, LV_OPA_0, LV_PART_MAIN);
      lv_obj_set_style_opa(red_rect_1, LV_OPA_0, LV_PART_MAIN);
      lv_obj_set_style_opa(red_rect_2, LV_OPA_0, LV_PART_MAIN);
    }
    else
    {
      // Fond blanc fixe quand distance >= 10 cm
      lv_obj_set_style_bg_color(lv_screen_active(), lv_color_make(0xFF, 0xFF, 0xFF), LV_PART_MAIN);

      // Afficher les rectangles selon la distance comme avant
      lv_obj_set_style_opa(green_rect_1, distance > 50 ? LV_OPA_COVER : LV_OPA_0, LV_PART_MAIN);
      lv_obj_set_style_opa(green_rect_2, distance > 50 ? LV_OPA_COVER : LV_OPA_0, LV_PART_MAIN);

      lv_obj_set_style_opa(yellow_rect_1, distance > 30 ? LV_OPA_COVER : LV_OPA_0, LV_PART_MAIN);
      lv_obj_set_style_opa(yellow_rect_2, distance > 30 ? LV_OPA_COVER : LV_OPA_0, LV_PART_MAIN);

      lv_obj_set_style_opa(red_rect_1, distance > 10 ? LV_OPA_COVER : LV_OPA_0, LV_PART_MAIN);
      lv_obj_set_style_opa(red_rect_2, distance > 10 ? LV_OPA_COVER : LV_OPA_0, LV_PART_MAIN);

      // Reset toggle pour éviter couleur incohérente au prochain <10cm
      toggle_color = false;
      last_toggle_time = current_time;
    }

    Serial.print("Distance (moyenne sur 10 lectures): ");
    Serial.print(distance, 1);
    Serial.println(" cm");

    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(200));
  }
}
