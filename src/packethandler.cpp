#include "packethandler.hpp"

PacketHandler::PacketHandler(QObject *parent) :
    QObject(parent)
{
}

void PacketHandler::processPackets(QByteArray p_packets, QTcpSocket *p_socket)
{
    while(p_packets.size())
    {
        QByteArray packet = QByteArray(p_packets[1], Qt::Uninitialized);
        std::move(p_packets.begin(), p_packets.begin() + static_cast<int>(p_packets[1]), packet.begin());
        p_packets.remove(0, p_packets[1]);
        processPacket(packet, p_socket);
    }
}

QByteArray PacketHandler::makeFSPacket(std::list<Card *> &p_field)
{
    QByteArray packet;
    packet.append(FIELD_SYNCHRO);
    for(auto it = p_field.begin(); it != p_field.end(); ++it)
    {
        packet.append(char(**it));
    }
    packet.insert(1, packet.size() + 1);
    return packet;
}

QByteArray PacketHandler::makeClickPacket(QByteArray p_cards)
{
    QByteArray packet;
    packet.append(CLICK);
    packet.append(p_cards);
    packet.insert(1, packet.size() + 1);
    return packet;
}

QByteArray PacketHandler::makeScoresPacket(QByteArray p_scores)
{
    QByteArray packet;
    packet.append(SCORES);
    packet.append(p_scores);
    packet.insert(1, packet.size() + 1);
    return packet;
}

QByteArray PacketHandler::makeGameStartedPacket()
{
    QByteArray packet;
    packet.append(GAME_STATE);
    packet.append(1);
    packet.insert(1, packet.size() + 1);
    return packet;
}

QByteArray PacketHandler::makeGameFinishedPacket()
{
    QByteArray packet;
    packet.append(GAME_STATE);
    packet.append(2);
    packet.insert(1, packet.size() + 1);
    return packet;
}

//QByteArray PacketHandler::makeWaitTimePacket(unsigned int p_waitTime)
//{
//    QByteArray packet;
//    packet.append(WAIT_TIME);
//    for(int i = sizeof(p_waitTime) - 1; i >= 0; --i)
//    {
//        packet.append(static_cast<char>((p_waitTime >> (i * 8)) & 0xFF));
//    }
//    packet.insert(1, packet.size() + 1);
//    return packet;
//}

QByteArray PacketHandler::makeDeckLengthPacket(short p_deckLength)
{
    QByteArray packet;
    packet.append(DECK);
    packet.append(p_deckLength);
    packet.insert(1, packet.size() + 1);
    return packet;
}

QByteArray PacketHandler::makeInputLockedPacket()
{
    QByteArray packet;
    packet.append(INPUT_STATE);
    packet.append(2);
    packet.insert(1, packet.size() + 1);
    return packet;
}

QByteArray PacketHandler::makeInputUnlockedPacket()
{
    QByteArray packet;
    packet.append(INPUT_STATE);
    packet.append(1);
    packet.insert(1, packet.size() + 1);
    return packet;
}

QByteArray PacketHandler::makeTurnPacket()
{
    QByteArray packet;
    packet.append(PLAYER_TURN);
    packet.insert(1, packet.size() + 1);
    return packet;
}

void PacketHandler::processPacket(QByteArray p_packet, QTcpSocket *p_socket)
{
    if(p_packet.size())
    {
        QByteArray packet(p_packet[1] - 2, Qt::Uninitialized);;
        std::copy(p_packet.begin() + 2, p_packet.end(), packet.begin());
        //unsigned int result = 0;
        switch(p_packet[0])
        {
        case FIELD_SYNCHRO:
            emit readField(packet);
            break;

        case CLICK:
            emit readClick(p_socket, packet);
            break;

        case SCORES:
            emit readScores(packet);
            break;

        case GAME_STATE:
            if(static_cast<short>(packet[0]) == 1)
                emit readGameStartedPacket();
            else if(static_cast<short>(packet[0]) == 2)
                emit readGameFinishedPacket();
            break;

        case DECK:
            emit readDeckLength(static_cast<short>(packet[0]));
            break;

        case INPUT_STATE:
            if(static_cast<short>(packet[0]) == 1)
                emit readUnlockedPacket();
            else if(static_cast<short>(packet[0]) == 2)
                emit readLockedPacket();
            break;

        case PLAYER_TURN:
            emit readTurnPacket(p_socket);
            break;

//        case WAIT_TIME:
//            for(int i = 0; i < packet.size(); i++)
//            {
//                result |= ((static_cast<unsigned int>(packet[i]) & 0xFF) << ((packet.size() - 1 - i) * 8));
//            }
//            emit readWaitTime(result);
//            break;

        default:
            std::cerr << "Dieser Pakettyp existiert nicht!" << std::endl;
        }
    }
}
