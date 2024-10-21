#include "modbustcp.h"


modbusTcp::modbusTcp(QObject *parent) : QObject(parent)
{
    transactionID = (quint16)0x0000;
    UnitId = (quint8)0x00;

    // création de la socket
    pSocket=new QTcpSocket (this);

    // association des evenements possible liés à la socket
    connect( pSocket, SIGNAL( connected()),    this, SLOT (onQTcpSocket_connected()));
    connect( pSocket, SIGNAL( disconnected()), this, SLOT (onQTcpSocket_disconnected()));
    connect( pSocket, SIGNAL( error (QAbstractSocket::SocketError)), this, SLOT (onQTcpSocket_error (QAbstractSocket::SocketError)));
    connect( pSocket, SIGNAL( readyRead()),    this, SLOT(onQTcpSocket_readyRead()));

}

// Function 03 (03hex) Read holding Registers
// Read the binary contents of input registers in the slave.

void modbusTcp::ReadHoldingRegisters(quint16 adresse, quint16 nb)
{

    registreAdresse = adresse;
    QByteArray buffer;
    QDataStream streamOut(&buffer, QIODevice::WriteOnly);
    streamOut << transactionID;
    streamOut << PROTOCOL_ID;
    streamOut << (quint16)0x0006;   //longueur de la trame PDU
    streamOut << UnitId;
    streamOut << (quint8)READ_HOLDING_REGISTERS;
    streamOut << (quint16)adresse;  // adresse du premier mot à lire
    streamOut << (quint16)nb;       // nombre de mot à lire
    //qDebug() << " >> " << buffer;
    transactionID++;                // le code de transaction est incrémenté
    pSocket->write(buffer);

}


// Function 06 (06hex) Preset Single Register
// Writes a value into a single holding register.

void modbusTcp::PresetSingleRegister(quint16 adresse, quint16 valeur)
{

    registreAdresse = adresse;
    QByteArray buffer;
    QDataStream streamOut(&buffer, QIODevice::WriteOnly);
    streamOut << transactionID;
    streamOut << PROTOCOL_ID;
    streamOut << (quint16)0x0006;   //longueur de la trame PDU
    streamOut << UnitId;
    streamOut << (quint8)PRESET_SINGLE_REGISTER;
    streamOut << (quint16)adresse;  // adresse du mot
    streamOut << (quint16)valeur;   // la valeur à écrire
    //qDebug() << " >> " << buffer;
    transactionID++;                // le code de transaction est incrémenté
    pSocket->write(buffer);

}



// connexion au serveur
void modbusTcp::ConnecterEsclaveModBus(QString adresse, qint16 port, quint8 slaveId)
{
    UnitId = slaveId;
    qDebug() << "unit Id " << UnitId;
    pSocket->connectToHost ( adresse, port );
}

// déconnexion du serveur
void modbusTcp::DeconnecterEsclaveModBus()
{
     pSocket->disconnectFromHost();
     pSocket->close();
}

void modbusTcp::onQTcpSocket_connected()
{
    connecte = true;
    emit SignalEtatConnexion("Connecté au serveur");
    qDebug() << "Connecté au serveur";
}

void modbusTcp::onQTcpSocket_disconnected()
{
    connecte = false;
    emit SignalEtatConnexion("Déconnecté du serveur");
    qDebug() << "Déconnecté";
}

void modbusTcp::onQTcpSocket_error(QAbstractSocket::SocketError socketError)
{
    QString message = "Modbus : Oups une erreur !!! <br />" + pSocket->errorString();
    emit SocketErreur(socketError);
    qDebug() << message;
}

 int modbusTcp::getregistreAdresse()
 {
     return registreAdresse;
 }

void modbusTcp::onQTcpSocket_readyRead()
{

    QDataStream fluxIn(pSocket);
    quint16 transactionId;
    quint16 ProtocolId;
    quint16 length;
    quint8 UnitId;
    quint8 fonctionCode;
    quint16 donnee;
    quint8 val;
    quint8 nb;
    QByteArray data;
    QString hexValue;
    QString decValue;
    int nbreMots;


    if ( pSocket->bytesAvailable() > 6 )
    {
        fluxIn >> transactionId;
        fluxIn >> ProtocolId;
        fluxIn >> length;
        fluxIn >> UnitId;
        fluxIn >> fonctionCode;
    }
    switch (fonctionCode)
    {

    case READ_HOLDING_REGISTERS:
        fluxIn >> nb;

        nbreMots = nb/2;
        for(int i =0; i<nbreMots; i++)
        {
            registreAdresse += i;
            fluxIn >> donnee;
            decValue = QString("%1").arg(donnee);
            emit Reponse(03,donnee,"(F3) Read holding registers ("+QString::number(registreAdresse) +") :"+ decValue);
        }

        break;

    case PRESET_SINGLE_REGISTER: // Preset Single Register
        emit Reponse(06,00,"(F6) Ecriture Registre effectuée ("+ QString::number(registreAdresse) + ")");
        break;


    default:

        fluxIn >> val;
        switch (val)
        {
        case ILLEGAL_FUNCTION_CODE:
            emit ModbusErreur(fonctionCode, val, "Erreur : La fonction demandée n'est pas supportée");
            break;
        case ILLEGAL_ADDRESS:
            emit ModbusErreur(fonctionCode, val, "Erreur : Adresse invalide");
            break;
        case ILLEGAL_DATA_VALUE:
            emit ModbusErreur(fonctionCode, val, "Erreur : Valeur hors de la plage admissible");
            break;
        case SERVER_DEVICE_BUSY:
            emit ModbusErreur(fonctionCode, val, "Erreur : le serveur est occupée");
            break;
        case MEMORY_PARITY_ERROR:
            emit ModbusErreur(fonctionCode, val, "Erreur : Valeur en lecture seule");
            break;
        case GATEWAY_PATH_UNAVAILABLE:
            emit ModbusErreur(fonctionCode, val, "Erreur : the  gateway  was  unable  to  allocate an internal communication");
            break;
        case GATEWAY_TARGET_DEVICE_FAILED_RESPOND:
            emit ModbusErreur(fonctionCode, val, "Erreur : from  gateway   no  response  was  obtained  from  the target device");
            break;
        default:
            emit ModbusErreur(fonctionCode, val, "Erreur : type inconnue");

        }

    }
    fluxIn >> data;


}
