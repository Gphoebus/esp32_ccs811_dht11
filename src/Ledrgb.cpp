#include <Arduino.h>
#include <Ledrgb.h>

Ledrgb::Ledrgb(int pin_r, int pin_g, int pin_b)
{
    _broches[0] = pin_r;
    _broches[1] = pin_g;
    _broches[2] = pin_b;
    for (int i = 0; i < 3; i++)
    {
        ledcAttachPin(_broches[i], _cannaux[i]); // broche 18 associée au canal PWM 0
        ledcSetup(_cannaux[i], 1200, 8);         // canal = 0, frequence = 5000 Hz, resolution = 12 bits
    }
}
void Ledrgb::clignote()
{
    int i = 0;
    while (i < 4)
    {
        if (millis() - _previousMillis >= _interval)
        {
            _previousMillis = millis();
            ledcWrite(_cannaux[i], 255);
            i = i + 1;
        }
    }
}
void Ledrgb::off()
{
    ledcWrite(_cannaux[0], 0);
    ledcWrite(_cannaux[1], 0);
    ledcWrite(_cannaux[2], 0);
}
/**
 * @brief
 * allumme ou eteint la led rouge
 * @param etat
 * booleen true ou false
 */
void Ledrgb::rouge(boolean etat)
{
    if (etat == true)
    {
        ledcWrite(_cannaux[0], 255);
        ledcWrite(_cannaux[1], 0);
        ledcWrite(_cannaux[2], 0);
    }
    else
    {
        ledcWrite(_cannaux[0], 0);
        ledcWrite(_cannaux[1], 0);
        ledcWrite(_cannaux[2], 0);
    }
}
/**
 * @brief
 * illumine en rouge avec une valeur comprise entre 0 et 255
 *
 * @param intensite
 * intensite de 0 à 255
 */
void Ledrgb::rouge(int intensite)
{
    ledcWrite(_cannaux[0], intensite);
    ledcWrite(_cannaux[1], 0);
    ledcWrite(_cannaux[2], 0);
}

void Ledrgb::verte(boolean etat)
{
    if (etat == true)
    {
        ledcWrite(_cannaux[1], 255);
        ledcWrite(_cannaux[0], 0);
        ledcWrite(_cannaux[2], 0);
    }
    else
    {
        ledcWrite(_cannaux[1], 0);
        ledcWrite(_cannaux[0], 0);
        ledcWrite(_cannaux[2], 0);
    }
}
void Ledrgb::verte(int intensite)
{
    ledcWrite(_cannaux[1], intensite);
    ledcWrite(_cannaux[0], 0);
    ledcWrite(_cannaux[2], 0);
}
void Ledrgb::bleue(boolean etat)
{
    if (etat == true)
    {
        ledcWrite(_cannaux[2], 255);
        ledcWrite(_cannaux[1], 0);
        ledcWrite(_cannaux[0], 0);
    }
    else
    {
        ledcWrite(_cannaux[2], 0);
        ledcWrite(_cannaux[1], 0);
        ledcWrite(_cannaux[0], 0);
    }
}
void Ledrgb::bleue(int intensite)
{
    ledcWrite(_cannaux[2], intensite);
    ledcWrite(_cannaux[1], 0);
    ledcWrite(_cannaux[0], 0);
}
void Ledrgb::rouge_clignote(int nbr, unsigned long duree)
{
    int i = 0;
    boolean etat = true;
    while (i < nbr * 2)
    {
        if (millis() - _previousMillis >= duree)
        {
            _previousMillis = millis();
            if (etat == true)
            {
                rouge(true);
                verte(false);
                bleue(false);
            }
            else
            {
                rouge(false);
                verte(false);
                bleue(false);
            }

            i = i + 1;
            etat = !etat;
        }
    }
}
void Ledrgb::verte_clignote(int nbr, unsigned long duree)
{
    int i = 0;
    boolean etat = true;
    while (i < nbr * 2)
    {
        if (millis() - _previousMillis >= duree)
        {
            _previousMillis = millis();
            if (etat == true)
            {
                verte(true);
                rouge(false);
                bleue(false);
            }
            else
            {
                verte(false);
                rouge(false);
                bleue(false);
            }

            i = i + 1;
            etat = !etat;
        }
    }
}
void Ledrgb::bleue_clignote(int nbr, unsigned long duree)
{
    int i = 0;
    boolean etat = true;
    while (i < nbr * 2)
    {
        if (millis() - _previousMillis >= duree)
        {
            _previousMillis = millis();
            if (etat == true)
            {
                bleue(true);
                rouge(false);
                verte(false);
            }
            else
            {
                bleue(false);
                rouge(false);
                verte(false);
            }

            i = i + 1;
            etat = !etat;
        }
    }
}
/**
 * @brief
 * allume ou éteind la led en orange
 *
 * @param etat
 * booleen true ou false
 */
void Ledrgb::orange(boolean etat)
{
    if (etat == true)
    {
        ledcWrite(_cannaux[0], 255);
        ledcWrite(_cannaux[1], 128);
        ledcWrite(_cannaux[2], 0);
    }
    else
    {
        ledcWrite(_cannaux[0], 0);
        ledcWrite(_cannaux[1], 0);
        ledcWrite(_cannaux[2], 0);
    }
}