#include "buy_ticket.h"
#include "bns.h"
#include "packets.h"
#include "action.h"

bot::BuyTicket::BuyTicket() {
}

bot::BuyTicket::~BuyTicket() {
}

bool bot::BuyTicket::Execute() {
	// We assume that you stand in front of the wheel.
	bns::Bns *bns_instance = bns::Bns::getInstance();
	bns_instance->SendPacketEasy((void *)packets::buy_orb);
	// TODO: wait for inventory event of orb.. but meh not now. Not yet.
	Sleep(500);
	UIF();
	Sleep(200);
	Nothing();
	return false;
}
