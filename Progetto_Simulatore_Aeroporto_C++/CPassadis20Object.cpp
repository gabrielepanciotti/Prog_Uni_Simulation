#include "CPassadis20Object.h"
#include "simulator.h"
#include "simulationobject.h"
#include "simulationevent.h"
#include "passenger.h"
#include "structs.h"
#include <string>
#include <random>
#include <iostream>
#include<fstream>
#include <ctime>

using namespace std;
ofstream fout;


CPassadis20Object::CPassadis20Object(CSimulator* simulator, int category, int id, string nom, int capacidad) :CSimulationObject(simulator, category, id, nom)
{
	ofstream fout1;
	fout1.open("miofile"+this->getName()+".txt");
	cout << "Creation passadis\nCapacidad: "+to_string(capacidad)+"\n";
	fout1 << "Creation passadis\nCapacidad: "+to_string(capacidad)+"\n";
	setState(IDLE);
	setCapacidad(capacidad);
	capacidad_actual = capacidad;
	fout1.close();
}

bool CPassadis20Object::AcceptEntity(CSimulationObject* emissor) {
	fout.open("miofile"+this->getName()+".txt", std::ios_base::app);
	fout << "=======ACCEPTENTITY========\n";
	fout << "Passadis: "+this->getName()+"\n";
	if (accepted_not_push < capacidad_actual){
		accepted_not_push++;
		fout << "The passadis accept the entity\n";
		fout << "Capacidad actual:"+to_string(capacidad_actual)+"\n";
		fout << "Entity accepted ma not received:"+to_string(accepted_not_push)+"\n";
		fout << "===============\n";
		fout.close();
		return true;
	}
	else {
		fout << "The passadis NOT accept the entity\n Emissore: "+emissor->getName()+"\n";
		fout << "Capacidad actual:"+to_string(capacidad_actual)+"\n";
		fout << "Entity accepted ma not received:"+to_string(accepted_not_push)+"\n";
		lista_pendientes_aceptaciones.push_back(emissor);
		fout << "===============\n";
		fout.close();
		return false;
	}
}

bool CPassadis20Object::SendMeNow(CSimulationObject* tincEspai) {
	fout.open("miofile"+this->getName()+".txt", std::ios_base::app);
	fout << "=======SENDMENOW========\n";
	fout << "Passadis: "+this->getName()+"\n";
	fout << "The receiver"+tincEspai->getName()+"call SendMeNow\n";
	list<CPassenger*> passengers = lista_pendientes[tincEspai];
	if(!passengers.empty()) {
		CEntity* ent = passengers.front();
		fout << "Passenger push:"+to_string(ent->getId())+"\n";
		CSimulationEvent* eventPush = new CSimulationEvent(getSimulator()->time(), this, tincEspai, ent, ePUSH);
		passengers.pop_front();
		m_Simulator->scheduleEvent(eventPush);
		nSendMeNow++;
		passanger_pendientes_actual--;
		fout << "===============\n";
		fout.close();
		return true;
	}
	else {
		fout << "===============\n";
		fout.close();
		return false;
	}

}

//Métode que el simulador us invocarà per a recollir els estadístics (print per consola)
void CPassadis20Object::showStatistics() {
	//Tempo medio, capacità media
	fout.open("miofile"+this->getName()+".txt", std::ios_base::app);
	fout << getName() << " Estadísticas Passsadis N.20 \n";
	fout << " Numero de pasajeros que han pasado por el passadis: "+ to_string(passenger_total) + "\n";
	fout << " Numero máximo de pasajeros en el passadis: " + to_string(passenger_max) + "\n";
	for (size_t i = 0; i < passenger_total; i++)
	{
		passenger_average += passenger_in_passadis[i];
	}
	passenger_average /= passenger_in_passadis.size();
	fout << " Numero medio de pasajeros en el passadis: " + to_string(passenger_average) + "\n";
	fout << " Tiempo maximo de un pasajero en el passadis: " + to_string(time_max) + "\n";
	for (size_t i = 0; i < time_in_passadis.size(); i++)
	{
		time_average += time_in_passadis[i];
	}
	time_average /= time_in_passadis.size();
	fout << " Tiempo medio de un pasajero en el passadis: " + to_string(time_average) + "\n";
	fout << " Numero de pasajeros pendientes en total: " + to_string(passanger_pendientes_total) + "\n";
	fout << " Numero de pasajeros pendientes maximo: " + to_string(passanger_pendientes_max) + "\n";
	fout << " Numero de veces que el passadis se ha puesto FULL: " + to_string(nStateFull) + "\n";
	fout << " Numero de veces que el passadis llamó SendMeNow: " + to_string(nSendMeNow) + "\n";
	fout.close();
};

//Processar un esdeveniment de simulació, funció pura que us toca implementar
void CPassadis20Object::processEvent(CSimulationEvent* event) {
	switch (getState()) {
	//Si el objeto está en estado IDLE
	case IDLE:
		//Si el evento recibido es de tipo PUSH, Significa que el objeto que envió el PUSH, 
		//ya ha verificado con acceptEntity() que el receptor puede quedarse con el pasajero
		if (event->getEventType() == ePUSH) {
			fout.open("miofile"+this->getName()+".txt", std::ios_base::app);
			fout << "===============\n";
			fout << "Passadis: "+this->getName()+"\n";
			fout << "Event ePUSH, Entity: " + to_string(event->getEntity()->getId())+"\n";
			
			int randomNumber = (rand() % 35) + 1;
			float time_left_passadis = getSimulator()->time() + randomNumber;
			fout << "Time in passadis1: " + to_string(time_left_passadis)+"\n";
			if (((CPassenger*)event->getEntity())->takeFlight()){
				int nFinger=((CPassenger*)event->getEntity())->getNumberFinger();
				float time_to = m_Simulator->timeTo(m_Simulator->getFinger(nFinger), ((CPassenger*)event->getEntity())); //Finger or nextObject?
				float time_left = ((CPassenger*)event->getEntity())->getDepartureTime() - m_Simulator->time();
				time_left_passadis = time_left - time_to;
				//Si el tiempo restante <0--> Avión perdido
				if (time_left_passadis < 0)
					m_Simulator->losesFlight(((CPassenger*)event->getEntity()),OVERTIME);				
			}
			time_in_passadis.push_back(time_left_passadis);
			fout << "Time in passadis: " + to_string(time_left_passadis)+"\n";
			fout.close();

			//Crear y programar evento ENDSERVICE
			CSimulationEvent* eventEndService = new CSimulationEvent(time_left_passadis,this, this, event->getEntity(), eSERVICE);
			m_Simulator->scheduleEvent(eventEndService);
			
			fout.open("miofile"+this->getName()+".txt", std::ios_base::app);
			fout << "Passadis "+this->getName()+" Programo un eSERVICE per a l'entitat " + to_string(event->getEntity()->getId()) + "\n";
			//un nuevo pasajero ha llegado al pasillo, su capacidad se reduce entonces
			if(time_left_passadis>time_max)
				time_max=time_left_passadis;
			
			passenger_total++;
			capacidad_actual--;
			accepted_not_push--;
			fout << "Capacidad actual: " + to_string(capacidad_actual)+"\n";
			
			int nPassangers = capacidad - capacidad_actual;
			passenger_in_passadis.push_back(nPassangers);
			if (nPassangers > passenger_max)
				passenger_max = nPassangers;
			//Si la capacidad está a 0, pasa al estado FULL
			if (capacidad_actual == 0) {
				setState(FULL);
				nStateFull++;
				fout << " Passadis: esta ple\n";
			}
			fout << "===============\n";
			fout.close();
		}
		
		if (event->getEventType() == eSERVICE) {
			fout.open("miofile"+this->getName()+".txt", std::ios_base::app);
			//Con nextObject encuentra los posibles destinos
			//a los que puede ir el pasajero a través del pasillo, los guarda en una list
			std::list<struct__route> destins;
			destins = m_Simulator->nextObject(event->getEntity(), this); 
			struct__route  destino = destins.front();
			fout << "===============\n";
			fout << "Passadis: "+this->getName()+"\n";
			fout << "Event eSERVICE, Entity: " + to_string(event->getEntity()->getId())+"\n";
			fout << "Destino: "+destino.destination->getName()+"\n";
			fout << "===============\n";
			fout.close();
			if (destino.destination->AcceptEntity(this)) { 
				fout.open("miofile"+this->getName()+".txt", std::ios_base::app);
				fout << "===============\n";
				fout << "Passadis "+this->getName()+" Programo un ePUSH per a l'entitat " + to_string(event->getEntity()->getId()) + "\n";
				capacidad_actual++;
				int nPassangers = capacidad - capacidad_actual;
				passenger_in_passadis.push_back(nPassangers);
				fout << "Capacidad actual: " + to_string(capacidad_actual)+"\n";
				fout << "===============\n";
				fout.close();
				
				CSimulationEvent* eventPush = new CSimulationEvent(event->getTime(), this, destino.destination, event->getEntity(), ePUSH);
				m_Simulator->scheduleEvent(eventPush);
				
				fout.open("miofile"+this->getName()+".txt", std::ios_base::app);
				//Si hay algún pasajero en espera de ser aceptado, se le llama con sendMeNow()
				CSimulationObject* pendiente = NULL;
				pendiente = lista_pendientes_aceptaciones.front();
				if (pendiente != NULL) {
					fout << "pendiente:"+pendiente->getName()+"\n";
					fout << "Send Me Now invocated\n";
					fout << "===============\n";
					fout.close();
					pendiente->SendMeNow(this);
				}
				else{
					fout << "pendiente idle\n";
					fout.close();
				}
			}
			else {
				fout.open("miofile"+this->getName()+".txt", std::ios_base::app);
				fout << "===============\n";
				fout << "Entity NOT accepted\n";
				fout << "Passanger pendiente: "+to_string(event->getEntity()->getId())+"\n";
				fout << "Capacidad actual: " + to_string(capacidad_actual)+"\n";
				fout << "===============\n";
				passanger_pendientes_total++;
				passanger_pendientes_actual++;
				if (passanger_pendientes_actual > passanger_pendientes_max)
					passanger_pendientes_max = passanger_pendientes_actual;
				lista_pendientes[destino.destination].push_back((CPassenger *)event->getEntity());
				setState(IDLE);
				fout.close();
			}
		}
		break;
	case FULL:
		if (event->getEventType() == eSERVICE) {
			fout.open("miofile"+this->getName()+".txt", std::ios_base::app);
			std::list<struct__route> destins;
			destins = m_Simulator->nextObject(event->getEntity(), this);
			struct__route  destino = destins.front();
			fout << "===============\n";
			fout << "Passadis: "+this->getName()+"\n";
			fout << "Event eSERVICE, Entity: " + to_string(event->getEntity()->getId())+"\n";
			fout << "State FULL\n";
			fout << "Destino: "+destino.destination->getName()+"\n";
			fout << "===============\n";
			fout.close();
			if (destino.destination->AcceptEntity(this)) {
				fout.open("miofile"+this->getName()+".txt", std::ios_base::app);
				fout << "===============\n";
				fout << "Passadis "+this->getName()+" Programo un ePUSH per a l'entitat " + to_string(event->getEntity()->getId()) + "\n";
				capacidad_actual++;
				int nPassangers = capacidad - capacidad_actual;
				passenger_in_passadis.push_back(nPassangers);	
				fout << "Capacidad actual: " + to_string(capacidad_actual)+"\n";
				setState(IDLE);
				fout.close();

				CSimulationEvent* eventPush = new CSimulationEvent(event->getTime(), this, destino.destination, event->getEntity(), ePUSH);
				m_Simulator->scheduleEvent(eventPush);
				
				fout.open("miofile"+this->getName()+".txt", std::ios_base::app);
				//Si hay algún pasajero en espera de ser aceptado, se le llama con sendMeNow()
				CSimulationObject* pendiente = NULL;
				pendiente = lista_pendientes_aceptaciones.front();
				if (pendiente != NULL) {
					fout << "Send Me Now invocated\n";
					fout << "===============\n";
					fout << "Pendiente:"+pendiente->getName()+"\n";
					fout.close();
					pendiente->SendMeNow(this);
				}
				else{
					fout << "Pendiente IDLE\n";
					fout.close();
				}
			}
			else {
				fout.open("miofile"+this->getName()+".txt", std::ios_base::app);
				fout << "===============\n";
				fout << "Entity NOT accepted\n";
				fout << "Passanger pendiente: "+to_string(event->getEntity()->getId())+"\n";
				fout << "Capacidad actual: " + to_string(capacidad_actual)+"\n";
				fout << "===============\n";
				passanger_pendientes_total++;
				passanger_pendientes_actual++;
				if (passanger_pendientes_actual > passanger_pendientes_max)
					passanger_pendientes_max = passanger_pendientes_actual;
				std::list<CPassenger*> passengers = lista_pendientes.find(destino.destination)->second;
				if (passengers.size() > 0) {
					list<CPassenger*> myVector;
					myVector.push_front((CPassenger*)event->getEntity());
					lista_pendientes[destino.destination] = myVector;
				}
				else {
					passengers.push_back((CPassenger*)event->getEntity());
					lista_pendientes[destino.destination] = passengers;
				}
				setState(FULL);
				fout.close();
			}
		}
		break;
	}
	fout.close();
}
	//Métode que el simulador invocarà a l'inici de la simulació, abans de que hi hagi cap esdeveniment a la llista d'esdeveniments
void CPassadis20Object::simulationStart() {
	fout.open("miofile"+this->getName()+".txt", std::ios_base::app);	setState(IDLE);
	passenger_average=0;
	passenger_max=0;
	passenger_total=0;
	time_average=0;
	time_in_passadis.clear();
	time_max=0;
	nSendMeNow=0;
	nStateFull=0;
	passanger_pendientes_max=0;
	passanger_pendientes_total=0;
	passanger_pendientes_actual = 0;
	accepted_not_push=0;
	srand((unsigned) time(0));

	cout << getName() << " inicialitzat \n";
	fout << getName() << " inicialitzat \n";
	fout.close();
}
	//Métode que el simulador us pot invocar a la finalització de l'estudi
void CPassadis20Object::simulationEnd() {
	setState(IDLE);
}
