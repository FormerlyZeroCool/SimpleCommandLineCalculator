/*
 * largeint.h
 *
 *  Created on: Oct 14, 2019
 *      Author: andrew
 */

#ifndef LARGEINT_H_
#define LARGEINT_H_
#include "unsortedlist.h"
#include <string>
#include <iostream>


struct divResult{
	unsortedList<short> quotient;
	short remainder;
	divResult(unsortedList<short> q,short r):quotient(q),remainder(r){}
};
//Big Endian
class largeInt{
private:
	unsortedList<short> number;
	bool negative;
	short radix;
	bool isZero();
	void trim();
	unsortedList<short> addTwoLists(unsortedList<short> &summand1,unsortedList<short> &summand2,short radix);
	//Will run as long as divisor is less than 2^15
	divResult divideList(unsortedList<short> dividend,short divisor,short radix);
public:
	largeInt getBinVersion();
	largeInt():radix(10),negative(false){}
	largeInt(std::string num);
	largeInt(int other);
	largeInt(unsortedList<short> num,bool isNegative = false,short radix = 10);
	bool operator<=(largeInt& o);
	void operator=(const largeInt& other);
	void operator=(int other);
	bool operator==(largeInt& other);
	bool operator!=(int other);
	void printNum();
	largeInt operator-(largeInt subtrahend);
	void operator-=(largeInt subtrahend);
	void operator*=(largeInt multiplyer);
	largeInt operator*(largeInt multiplyer);
	largeInt operator/(largeInt dividend);
	largeInt operator%(largeInt dividend);
	void operator+=(largeInt summand);
	largeInt operator+(largeInt summand);
	bool operator<(largeInt&);
	void setRadix(short radix);
	bool isNegative();
};


#endif /* LARGEINT_H_ */
