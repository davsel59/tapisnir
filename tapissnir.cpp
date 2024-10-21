#include "tapissnir.h"

#include "modbustcp.h"
#include <QThread>
#include <QTcpSocket>
#include <QErrorMessage>


tapisnir::tapisnir(QObject *parent) : QObject(parent) , bus(parent)
{
   
  
   

}


void tapisnir::ConnexionServeur(QString adresse, qint16 port, quint8 slaveId)
{
    bus.ConnecterEsclaveModBus(adresse,port,slaveId);

}

void tapisnir::Deconnexion()
{
    bus.DeconnecterEsclaveModBus();

}

void  tapisnir::OnEtatConnexion(QString message)
{
    emit Reponse(message);
}


void tapisnir::OnTcpErreur(QAbstractSocket::SocketError socketError)
{
     QString msg = "Erreur TCP" ;
     emit(ErreurTCP(msg));

}



void tapisnir::demarrerTapis()
{
    qint16 motRegistre = capteurs.registreCommandeTAPIS;
    motRegistre = motRegistre  | 768;
    bus.PresetSingleRegister(4525,motRegistre);

}

void tapisnir::arreterTapis()
{
    qint16 motRegistre = capteurs.registreCommandeTAPIS;
    motRegistre = motRegistre & 0xFCFF;
    bus.PresetSingleRegister(4525,motRegistre);

}

void tapisnir::modifierVitesseTapis(int vit)
{
    QString msg;

        bus.PresetSingleRegister(4525,vit);
        if(vit == 768)
            msg = " vitesse lente";
        if(vit == 1280)
            msg = " vitesse moyenne";
        if(vit == 256)
            msg = " vitesse rapide";

        emit(Reponse("Vitesse du tapis modifiée:" +msg ));
}

void tapisnir::OnErreur(quint8 code, quint8 value, QString reponse)
{
    emit(ErreurModbus(reponse));
}


void tapisnir::SortirVerinInjecteur()
{
    qDebug() << "Sortir verin injecteur";
    qint16 motRegistre = capteurs.registreCommandeVERINS;
    motRegistre = motRegistre + 1;
    bus.PresetSingleRegister(4526,motRegistre);
}

void tapisnir::RentrerVerinInjecteur()
{

   qDebug() << "Rentrer verin injecteur";
   qint16 motRegistre = capteurs.registreCommandeVERINS;
   motRegistre = motRegistre & 0xFFFE;
   bus.PresetSingleRegister(4526,motRegistre);

}

void tapisnir::SortirVerinEjecteur1()
{
    qDebug() << "Sortir verin ejecteur1";
   // a completer
}

void tapisnir::RentrerVerinEjecteur1()
{
    qDebug() << "Rentrer verin ejecteur1";
    // a completer
}

void tapisnir::SortirVerinEjecteur2()
{

    qDebug() << "Sortir verin ejecteur2";
    // a completer;

}


void tapisnir::RentrerVerinEjecteur2()
{
    qDebug() << "Rentrer verin ejecteur2";
    // a completer

}

void tapisnir::lectureRegistresTapisnir()
{
    switch (typeTrameMODBUS)
       {
               case  ETAT_DETECTEURS_PRESENCES :
                       bus.ReadHoldingRegisters(registreDetecteurPresenceColis,1);  // lecture du registre 4096
                       break;

               case  ETAT_VERINS :
                       bus.ReadHoldingRegisters(registreCapteursVerin,1);  // lecture du registre 4097
                       break;

               case IDLE :
                       typeTrameMODBUS=ETAT_DETECTEURS_PRESENCES;
                       break;

               case COMMANDE_VERIN :
                       bus.ReadHoldingRegisters(registreCommandeVerins,1);
                       break;

                 case ETAT_TAPIS :
                        typeTrameMODBUS=ETAT_DETECTEURS_PRESENCES;
                        break;

               case COMMANDE_TAPIS :
                         bus.ReadHoldingRegisters(registreCommandeTapis,1);
                         break;
       }

}

void tapisnir::OnReponse(quint8 code, quint16 value, QString reponse)
{
    if(code ==  bus.READ_HOLDING_REGISTERS)
    {
        if(typeTrameMODBUS == ETAT_DETECTEURS_PRESENCES)
         {
           capteurs.DetecteurPresenceColisPosteInjecteur = (value >> 3) & 1U;
           capteurs.DetecteurPresenceColisPosteEjecteur1 = (value >> 1) & 1U;
           capteurs.DetecteurPresenceColisPosteEjecteur2 = (value >> 2) & 1U;

           qDebug() << "DetecteurPresenceColisPosteInjecteur: " << capteurs.DetecteurPresenceColisPosteInjecteur;
           qDebug() << "DetecteurPresenceColisPosteEjecteur1: " << capteurs.DetecteurPresenceColisPosteEjecteur1;
           qDebug() << "DetecteurPresenceColisPosteEjecteur2: " << capteurs.DetecteurPresenceColisPosteEjecteur2;
           typeTrameMODBUS  = ETAT_VERINS;

         }
        else
        {
          if(typeTrameMODBUS == ETAT_VERINS )
          {
             capteurs.VerinIjecteur = (value >> 1) & 1U;
             capteurs.VerinEjecteur1 = (value >> 3) & 1U;
             capteurs.VerinEjecteur2 = (value >> 9) & 1U;

             qDebug() << "VerinIjecteur: " << capteurs.VerinIjecteur;
             qDebug() << "VerinEjecteur1: " <<capteurs.VerinEjecteur1;
             qDebug() << "VerinEjecteur2: " <<capteurs.VerinEjecteur2;
            typeTrameMODBUS = COMMANDE_VERIN;
            }
            else
            {
             if(typeTrameMODBUS == COMMANDE_VERIN )
             {
               capteurs.registreCommandeVERINS = value;
               qDebug() << "Registre Commmande verin: " << value;
               typeTrameMODBUS = COMMANDE_TAPIS;
             }
             else
             {
              if(typeTrameMODBUS == COMMANDE_TAPIS )
              {
                 capteurs.registreCommandeTAPIS = value;
                 qDebug() << "Registre Commmande TAPIS: " << value;
                 if(value != 0)
                 {
                     capteurs.EtatTapis = true;
                 }
                 else
                 {
                      capteurs.EtatTapis = false;
                 }
                 typeTrameMODBUS = IDLE;
               }
              }

             }
       }
        qDebug() <<value;
        qDebug() <<reponse;
    }

    if(code ==  bus.PRESET_SINGLE_REGISTER)
    {
        qDebug() <<reponse;
        qDebug() <<value;
    }


    emit(Reponse(reponse));

}

