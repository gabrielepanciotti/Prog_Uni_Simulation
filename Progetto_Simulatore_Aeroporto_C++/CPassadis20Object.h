#pragma once
#include "simulationobject.h"
#include "simulator.h"
#include <map>
#include <vector>
#include <list>



class CPassadis20Object:public CSimulationObject{
    int capacidad;
    int capacidad_actual;
    
    //para acceptEntity y SendMeNow
    std::map< CSimulationObject *,std::list <CPassenger *> > lista_pendientes;
    std::list< CSimulationObject* > lista_pendientes_aceptaciones;

    //para las estadísticas
    std::vector< float > time_in_passadis;
    std::vector< int > passenger_in_passadis;
    float time_average;
    float passenger_average;
    float time_max;
    int passenger_total;
    int passenger_max;
    int nSendMeNow;
    int nStateFull;
    int passanger_pendientes_total;
    int passanger_pendientes_max;
    int passanger_pendientes_actual;
    int accepted_not_push;

    public:
    CPassadis20Object(CSimulator* simulator,int category, int id, std::string nom, int capacidad);
    ~CPassadis20Object(){}
    //Método que avisa al emisor de que está preparado para recibir
    bool SendMeNow(CSimulationObject* tincEspai);
    //Métode que el simulador us invocarà per a recollir els estadístics (print per consola)
    void showStatistics();
    //És una funció virtial pura així que us tocarà implementar-la indiferentment de si la invoqueu o no.
    bool AcceptEntity(CSimulationObject* emissor);
    //Processar un esdeveniment de simulació, funció pura que us toca implementar
    void processEvent (CSimulationEvent* event);
    //Métode que el simulador invocarà a l'inici de la simulació, abans de que hi hagi cap esdeveniment a la llista d'esdeveniments
    void simulationStart();
    //Métode que el simulador us pot invocar a la finalització de l'estudi
    void simulationEnd();
    //setCapacidad
    void setCapacidad(int capacidad) {
        this->capacidad = capacidad;
    }
    int getCapacidad() {
        return capacidad;
    }

};
