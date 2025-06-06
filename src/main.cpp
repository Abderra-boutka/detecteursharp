#include "lvgl.h"

// === Nouveau : Label global pour afficher la distance ===
lv_obj_t *labelDistance;

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
  // Boutons
  lv_obj_t * label;

  lv_obj_t * btn1 = lv_button_create(lv_screen_active());
  lv_obj_add_event_cb(btn1, event_handler, LV_EVENT_ALL, NULL);
  lv_obj_align(btn1, LV_ALIGN_CENTER, 0, -40);
  lv_obj_remove_flag(btn1, LV_OBJ_FLAG_PRESS_LOCK);

  label = lv_label_create(btn1);
  lv_label_set_text(label, "Button");
  lv_obj_center(label);

  lv_obj_t * btn2 = lv_button_create(lv_screen_active());
  lv_obj_add_event_cb(btn2, event_handler, LV_EVENT_ALL, NULL);
  lv_obj_align(btn2, LV_ALIGN_CENTER, 0, 40);
  lv_obj_add_flag(btn2, LV_OBJ_FLAG_CHECKABLE);
  lv_obj_set_height(btn2, LV_SIZE_CONTENT);

  label = lv_label_create(btn2);
  lv_label_set_text(label, "Toggle");
  lv_obj_center(label);

  // Label pour la distance 
  labelDistance = lv_label_create(lv_screen_active());
  lv_label_set_text(labelDistance, "Distance : -- cm");
  lv_obj_align(labelDistance, LV_ALIGN_CENTER, 0, 100);
}

#ifdef ARDUINO

#include "lvglDrivers.h"

void mySetup()
{
  Serial.begin(115200); // Initialisation du port série
  Serial.println("Sharp");
  testLvgl();
  xTaskCreate(myTask, "SharpTask", 2048, NULL, 1, NULL); // Lancement de la tâche pour la distance
}

void loop()
{
  lv_timer_handler();
  delay(5);
}

void myTask(void *pvParameters)
{
  TickType_t xLastWakeTime = xTaskGetTickCount();
  int valeurs[10] = {0};  // Tableau pour stocker les 10 dernières valeurs
  int index = 0;          // Index pour le tableau circulaire
  int somme = 0;          // Somme des valeurs pour la moyenne

  while (1)
  {
    int valeur = analogRead(A0);               // Lecture valeur brute capteur (0-1023)
    
    // Mise à jour du tableau circulaire
    somme = somme - valeurs[index];            // Soustrait l'ancienne valeur
    valeurs[index] = valeur;                   // Stocke la nouvelle valeur
    somme = somme + valeur;                    // Ajoute la nouvelle valeur
    index = (index + 1) % 10;                  // Avance l'index de manière circulaire
    
    // Calcul de la moyenne
    float moyenne = somme / 10.0;
    
    // Calcul distance avec la moyenne
    float distance = 40797 * pow(valeur, -1.284);
    
    delay(1000);
    Serial.print("Distance : ");
    Serial.print(distance);                     // Affichage distance en cm
    Serial.println(" cm");

    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(200));
  }
}

#else

#include "lvgl.h"
#include "app_hal.h"
#include <cstdio>

int main(void)
{
  printf("LVGL Simulator\n");
  fflush(stdout);

  lv_init();
  hal_setup();

  testLvgl();

  hal_loop();
  return 0;
}

#endif
