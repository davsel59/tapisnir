// http://www.modbus.org/docs/Modbus_Application_Protocol_V1_1b3.pdf

#ifndef MODBUSTCP_H
#define MODBUSTCP_H

#include <QObject>
#include <QTcpSocket>
#include <QDebug>
#include <QDataStream>


class modbusTcp : public QObject
{
    Q_OBJECT
public:
    explicit modbusTcp(QObject *parent = nullptr);
    void PresetSingleRegister(quint16 adresse, quint16 valeur);
    void ReadHoldingRegisters(quint16 adresse, quint16 nb);

    void ConnecterEsclaveModBus(QString adresse, qint16 port, quint8 slaveId);
    void DeconnecterEsclaveModBus();
    int getregistreAdresse();
signals:
    void Reponse(quint8 fonction, quint16 value, QString expression);
    void ModbusErreur(quint8 fonction, quint8 codeErreur, QString expression);
    void SocketErreur(QAbstractSocket::SocketError socketError);
    void SignalEtatConnexion(QString etat);

public slots:
    void onQTcpSocket_connected();
    void onQTcpSocket_disconnected();
    void onQTcpSocket_error(QAbstractSocket::SocketError socketError);
    void onQTcpSocket_readyRead();

private:
    QTcpSocket *pSocket;
    qint16 transactionID;
    quint8 UnitId;
    bool connecte;
    int registreAdresse;


public:
    enum CODE_FONCTION{
        READ_COIL = 1,
        READ_DISCRETE_INPUTS = 2,
        READ_HOLDING_REGISTERS = 3,
        READ_INPUT_REGISTERS = 4,
        FORCE_SINGLE_COIL = 5,
        PRESET_SINGLE_REGISTER = 6,
        READ_EXCEPTION_STATUS = 7,
        DIAGNOSTIC = 8,
        WRITE_MULTIPLE_COILS = 15,
        WRITE_MULTIPLE_REGISTERS = 16,
        READ_WRITE_MULTIPLE_REGISTERS = 23,
        READ_DEVICE_IDENTIFICATION = 0x2B
    };

    enum CODE_EXCEPTION{
        ILLEGAL_FUNCTION_CODE = 1,
        ILLEGAL_ADDRESS,
        ILLEGAL_DATA_VALUE,
        SERVER_DEVICE_FAILURE,
        ACKNOWLEDGE,
        SERVER_DEVICE_BUSY,
        MEMORY_PARITY_ERROR = 8,
        GATEWAY_PATH_UNAVAILABLE = 0x0a,
        GATEWAY_TARGET_DEVICE_FAILED_RESPOND = 0x0b

    };
protected:
    const qint16 PROTOCOL_ID = 0x0000;

};

#endif // MODBUSTCP_H
