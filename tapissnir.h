#ifndef TAPISSNIR_H
#define TAPISSNIR_H

#include <QObject>
#include "modbustcp.h"

#define Sorti        1
#define Rentre       0
#define Demarre      1
#define Arrete       0

#define registreCapteursVerin 4097
#define registreDetecteurPresenceColis 4096
#define registreCommandeVerins   4526
#define registreCommandeTapis    4525

// structure Capteur permettant de connaitre l etat des capteurs du TapiSnir
struct Capteurs
{
    bool EtatTapis;
    bool VerinIjecteur;
    bool VerinEjecteur1;
    bool VerinEjecteur2;
    bool DetecteurPresenceColisPosteInjecteur;
    bool DetecteurPresenceColisPosteEjecteur1;
    bool DetecteurPresenceColisPosteEjecteur2;
    qint16 registreCommandeVERINS ;
    qint16 registreCommandeTAPIS ;


};


class tapisnir : public QObject
{
    Q_OBJECT
public:
    explicit tapisnir(QObject *parent = 0);

private :
    QString adresseIP;
    modbusTcp bus;
    int typeTrameMODBUS;


    // Enumeration permettant d'ordonner les echanges modBusTCP
    // Emission d'une trame et attente de la réponse correspondante
    enum TYPE_TRAME_MODBUS
       {
          IDLE = 0,
          ETAT_TAPIS = 1,
          ETAT_VERINS = 2,
          ETAT_DETECTEURS_PRESENCES = 3,
          COMMANDE_VERIN = 4,
          COMMANDE_TAPIS = 5,

       };

public :
   Capteurs capteurs;


public slots:
    void ConnexionServeur(QString adresse, qint16 port, quint8 slaveId);
    void Deconnexion();

    // methode de traitement des erreurs TCP
    void OnTcpErreur(QAbstractSocket::SocketError socketError);
    // methode appele precisant l etat de la connexion à la passerelle
    void OnEtatConnexion(QString message);
    // methode appele en cas de detection Erreur
    void OnErreur( quint8 code, quint8 value, QString reponse);
    // methode appele lorsqu'une reponse modbus tcp est arrivée
    void OnReponse(quint8 code, quint16 value, QString reponse);

    // methode permettant la mise en route du tapis
    void demarrerTapis();
    // methode permettant l'arret du tapis
    void arreterTapis();
    // methode permettant de modifier la vitesse du tapis
    void modifierVitesseTapis(int);
    // methodes permettant de contrôler chaque verin
    void SortirVerinInjecteur();
    void RentrerVerinInjecteur();
    void SortirVerinEjecteur1();
    void RentrerVerinEjecteur1();
    void SortirVerinEjecteur2();
    void RentrerVerinEjecteur2();

    // méthode chargée de mettre a jour la structure capteur
    void lectureRegistresTapisnir();



signals:
     // a utiliser pour prévenir l'IHM
     void Reponse(QString expression);
     void ErreurTCP(QString expression);
     void ErreurModbus(QString expression);



};

#endif // TAPISSNIR_H
