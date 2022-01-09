#ifndef Ledrgb_h
#define Ledrgb_h
#include <Arduino.h>

class Ledrgb
{
public:
    Ledrgb(int pin_R, int pin_g, int pin_b);
    void init();
    void clignote();
    void off();
    void rouge(boolean etat);
    void rouge(int intensite);
    void verte(boolean etat);
    void verte(int intensite);    
    void bleue(boolean etat);
    void bleue(int intensite);          
    void rouge_clignote(int nbr,unsigned long duree);
    void verte_clignote(int nbr,unsigned long duree);
    void bleue_clignote(int nbr,unsigned long duree);
    void orange(boolean etat);
private:
    int _broches[3] = {};
    int _cannaux[3] = {0, 1, 2};
    unsigned long _previousMillis = 0;
    unsigned long _interval = 500L;
};
#endif