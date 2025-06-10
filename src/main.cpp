#include <Arduino.h>
#include <math.h>

void setup()
{
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n=== DÉMARRAGE DU PROGRAMME ===");
  Serial.println("Capteur Sharp connecté sur la broche A0");
  Serial.println("--------------------------------");
}

void loop()
{
  static int valeurs[10] = {0}; // Tableau pour stocker les 10 dernières valeurs
  static int index = 0;        // Index pour le tableau circulaire
  static long somme = 0;       // Somme des valeurs dans le tableau

  // Lecture du capteur
  int valeur = analogRead(A0);
  
  // Mise à jour de la somme et du tableau circulaire
  somme = somme - valeurs[index]; // Soustraire l'ancienne valeur
  valeurs[index] = valeur;        // Ajouter la nouvelle valeur
  somme = somme + valeur;         // Mettre à jour la somme
  index = (index + 1) % 10;       // Passer à l'index suivant (circulaire)
  
  // Calcul de la moyenne
  float moyenne = somme / 10.0;
  
  // Calcul de la distance avec la moyenne
  float distance = 40797 * pow(moyenne, -1.284);
  
  // Limitation de la distance
  if (distance < 0) distance = 0;
  if (distance > 80) distance = 80;
  
  // Affichage simple de la distance
  Serial.print("Distance (moyenne sur 10 lectures): ");
  Serial.print(distance, 1);
  Serial.println(" cm");

  // Délai entre les lectures
  delay(200);
}

