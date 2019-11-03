/*
 * largint.cpp
 *
 *  Created on: Oct 17, 2019
 *      Author: andrew
 */

#include "largeint.h"
//Will run as long as divisor is less than 2^15
divResult largeInt::divideList(unsortedList<short> dividend,short divisor,short radix)
{
	unsortedList<short> quotient;
	short current;
	short remainder = 0;
	bool addZero = false;
	for(iterator<short> it = dividend.begin();it != dividend.end(); it++)
	{
		current = *it+(remainder*radix);
		if(current != 0)
		{
			addZero = true;
		}
		if(addZero)
		{
			quotient.insertTail(current/divisor);
			remainder = current % divisor;
		}
	}
	return divResult(quotient,remainder);
}
unsortedList<short> largeInt::addTwoLists(unsortedList<short> &summand1,unsortedList<short> &summand2,short radix)
{
	unsortedList<short> endSum;
	iterator<short> other = summand2.rbegin();
	iterator<short> num = summand1.rbegin();
	short carry = 0;
	short sum = 0;
	while(other != nullptr && num != nullptr)
	{
		sum = (*other+*num+carry)%radix;
		carry = (*other+*num+carry)/radix;
		endSum.insertHead(sum);
		other--;
		num--;
	}
	while(other != nullptr)
	{
		endSum.insertHead((*other+carry)%radix);
		carry = (*other+carry)/radix;
		other--;
	}
	while(num != nullptr)
	{
		endSum.insertHead((*num+carry)%radix);
		carry = (*num+carry)/radix;
		num--;
	}
	if(carry)
		endSum.insertHead(1);
	return endSum;
}

unsortedList<short> subtractTwoLists(unsortedList<short> minuend,unsortedList<short> subtrahend, short radix)
{
	iterator<short> oIt = subtrahend.rbegin();
	iterator<short> tIt = minuend.rbegin();
	iterator<short> walkerTIt = tIt;
	unsortedList<short> result;
	short cr = 0;
	while(oIt != nullptr && tIt != nullptr)
	{
		if(*tIt < *oIt)
		{
			walkerTIt = tIt;
			walkerTIt--;
			while(walkerTIt != minuend.begin() && *walkerTIt == 0)
			{
				walkerTIt--;
			}
			*walkerTIt -= 1;
			walkerTIt++;
			while(walkerTIt != minuend.rbegin() && walkerTIt != tIt)
			{
				*walkerTIt = radix-1;
				walkerTIt++;
			}
			*tIt = (radix+*tIt);
		}
		cr = *tIt - *oIt;
		result.insertHead(cr);
		oIt--;
		tIt--;
	}

	while(tIt != nullptr)
	{
		result.insertHead(*tIt);
		tIt--;
	}
	return result;

}

bool isEqualToZero(unsortedList<short> number)
{
	bool isZero = true;
	for(short i : number)
	{
		if(i != 0)
			isZero = false;
	}
	return isZero;
}
void largeInt::trim()
{
	iterator<short> it = number.begin();
	while(it != nullptr && *it == 0)
	{
		it++;
		number.removeHead();
	}
}
largeInt largeInt::getBinVersion()
{
	divResult result(number,0);
	unsortedList<short> binRep;
	while(!isEqualToZero(result.quotient))
	{
		result = divideList(result.quotient,2,radix);
		binRep.insertHead(result.remainder);
	}

	largeInt binNum = largeInt(binRep,this->negative,2);
	return binNum;
}
bool largeInt::isZero()
{
	bool isZero = true;
	for(short i : number)
	{
		if(i != 0)
			isZero = false;
	}
	return isZero;
}

void largeInt::setRadix(short newRadix)
{

	divResult result(number,0);
	unsortedList<short> convertedNum;
	while(!isEqualToZero(result.quotient))
	{
		result = divideList(result.quotient,newRadix,this->radix);
		convertedNum.insertHead(result.remainder);
	}

	this->number = convertedNum;
	this->radix = newRadix;

}
void largeInt::operator+=(largeInt summand)
{
	bool isNeg = false;
	unsortedList<short> result;
	if(summand.isNegative() && this->isNegative())
	{
		result = addTwoLists(this->number,summand.number,this->radix);
		isNeg = true;
	}
	else if(!summand.isNegative() && !this->isNegative())
	{
		result = addTwoLists(this->number,summand.number,this->radix);
	}
	else if(summand.isNegative() && !this->isNegative())
	{
		if(summand < *this)
		{
			result = subtractTwoLists(this->number,summand.number,radix);
		}
		else
		{
			result = subtractTwoLists(summand.number,this->number,radix);
			isNeg = true;
		}
	}
	else if(!summand.isNegative() && this->isNegative())
	{
		if(summand < *this)
		{
			result = subtractTwoLists(this->number,summand.number,radix);
			isNeg = true;
		}
		else
		{
			result = subtractTwoLists(summand.number,this->number,radix);
		}
	}
	largeInt data(result,isNeg);
	data.trim();
	*this = data;
}

largeInt largeInt::operator+(largeInt summand)
{
	bool isNeg = false;
	unsortedList<short> result;
	if(summand.isNegative() && this->isNegative())
	{
		result = addTwoLists(this->number,summand.number,this->radix);
		isNeg = true;
	}
	else if(!summand.isNegative() && !this->isNegative())
	{
		result = addTwoLists(this->number,summand.number,this->radix);
	}
	else if(summand.isNegative() && !this->isNegative())
	{
		if(summand < *this)
		{
			result = subtractTwoLists(this->number,summand.number,radix);
		}
		else
		{
			result = subtractTwoLists(summand.number,this->number,radix);
			isNeg = true;
		}
	}
	else if(!summand.isNegative() && this->isNegative())
	{
		if(summand < *this)
		{
			result = subtractTwoLists(this->number,summand.number,radix);
			isNeg = true;
		}
		else
		{
			result = subtractTwoLists(summand.number,this->number,radix);
		}
	}
	largeInt data(result,isNeg);
	data.trim();
	return data;
}

bool largeInt::isNegative()
{
	return negative;
}
void largeInt::operator*=(largeInt multiplyer)
{
	unsortedList<short> *intermediaryNumbers = new unsortedList<short>[multiplyer.number.length()];
	int product = 1;
	int carry = 0;
	int count = 0;
	bool isNeg = false;
	if(multiplyer.negative)
		isNeg = !isNeg;
	if(this->negative)
		isNeg = !isNeg;
	//setSign=


	for(iterator<short>  oIt = multiplyer.number.rbegin();oIt != nullptr;oIt--)
	{
		short oNum = *oIt;
		unsortedList<short> *intProduct = intermediaryNumbers+count++;
		for(int i = 1;i < count;i++)
		{
			intProduct->insertTail(0);
		}
		for(iterator<short> tIt = this->number.rbegin();tIt != nullptr;tIt--)
		{
			short tNum = *tIt;
			product = ((oNum)*(tNum)+carry)%radix;
			carry = ((oNum)*(tNum)+carry)/radix;
			intProduct->insertHead(product);
		}
		if(carry != 0)
		{
			intProduct->insertHead(carry);
			carry = 0;
		}


	}
	unsortedList<short> finalProduct;
	for(int i = 0; i<multiplyer.number.length();i++)
	{
		finalProduct = addTwoLists(finalProduct,intermediaryNumbers[i],radix);
	}
	delete [] intermediaryNumbers;

	 number = finalProduct;
	 negative = isNeg;
}
largeInt largeInt::operator*(largeInt multiplyer)
{
	unsortedList<short> *intermediaryNumbers = new unsortedList<short>[multiplyer.number.length()];
	int product = 1;
	int carry = 0;
	int count = 0;
	bool isNegLocal = false;
	if(multiplyer.negative)
		isNegLocal = !isNegLocal;
	if(this->negative)
		isNegLocal = !isNegLocal;
	//setSign=


	for(iterator<short>  oIt = multiplyer.number.rbegin();oIt != nullptr;oIt--)
	{
		short oNum = *oIt;
		unsortedList<short> *intProduct = intermediaryNumbers+count++;
		for(int i = 1;i < count;i++)
		{
			intProduct->insertTail(0);
		}
		for(iterator<short>  tIt = this->number.rbegin();tIt != nullptr;tIt--)
		{
			short tNum = *tIt;
			product = ((oNum)*(tNum)+carry)%radix;
			carry = ((oNum)*(tNum)+carry)/radix;
			intProduct->insertHead(product);
		}
		if(carry != 0)
		{
			intProduct->insertHead(carry);
			carry = 0;
		}


	}
	unsortedList<short> finalProduct;
	for(int i = 0; i<multiplyer.number.length();i++)
	{
		finalProduct = addTwoLists(finalProduct,intermediaryNumbers[i],radix);
	}
	delete [] intermediaryNumbers;

	return largeInt(finalProduct,isNegLocal,radix);
}

largeInt largeInt::operator/(largeInt divisor)
{
	short originalDivisorRadix = divisor.radix,
			originalThisRadix = this->radix;

	bool isNegLocal = false;

	if(divisor.negative)
		isNegLocal = !isNegLocal;
	if(this->negative)
		isNegLocal = !isNegLocal;

	largeInt portionOfDividend("");
	portionOfDividend.setRadix(2);
	largeInt dividend = *this;
	iterator<short> divIt = dividend.number.begin();
	unsortedList<short> result;
	int insertCount = 0;
	divisor.setRadix(2);
	dividend.setRadix(2);
	std::cout<<"PreRun ";
	std::cout<<"Dividend ";
	dividend.printNum();
	std::cout<<"divisor ";
	divisor.printNum();

	while(!dividend.number.isEmpty() && insertCount<20)
	{
		insertCount++;
		std::cout<<std::endl<<"is PortDividend < Divisor: "<<(portionOfDividend < divisor);
		std::cout<<std::endl;

		while(divIt != nullptr && portionOfDividend < divisor)
		{
			if(!portionOfDividend.isZero() || *divIt != 0)
				portionOfDividend.number.insertTail(*divIt);
			divIt++;
			dividend.number.removeHead();
			result.printList();
			if(insertCount == 0)
				result.insertTail(0);
			else
				insertCount--;
			result.printList();
			std::cout<<"portionDividend len:"<<portionOfDividend.number.getLength()<<" num:";
			portionOfDividend.printNum();

			std::cout<<"divisor ";
			divisor.printNum();
		}
		std::cout<<"portionDividend len:"<<portionOfDividend.number.getLength()<<" num:";
		portionOfDividend.printNum();
		if(!portionOfDividend.isZero() && divIt != nullptr
		)
		{
			result.insertTail(1);
			//if(insertCount != 0)
			//	insertCount--;
			largeInt remainder = portionOfDividend-divisor;
			std::cout<<"remainder "<<portionOfDividend.radix<< " ";
			remainder.printNum();
			portionOfDividend.number.emptyList();

			{
				///trim subtraction result
				iterator<short> it = remainder.number.begin();
				while(it != nullptr && *it == 0)
				{
					it++;
					remainder.number.removeHead();
				}
			}

			//insert remainder in head of dividend
			//walk divIt back one place unless divIt.getLength() <= this->getLength()
			if(!remainder.isZero())
			{
				remainder.printNum();
				iterator<short> remainderIt = remainder.number.rbegin();
				while(remainderIt != nullptr)
				{
					portionOfDividend.number.insertHead(*remainderIt);
					//std::cout<<*remainderIt;
					insertCount++;
					remainderIt--;
				}
				portionOfDividend.trim();
				std::cout<<"\ninserted remainder into dividend\n";
			}

			std::cout<<"dividend hello ";
			dividend.printNum();
		}
		else if(divIt == nullptr)
		{
			if (portionOfDividend < divisor || portionOfDividend.isZero())
					{
						//dividend = largeInt("");
						result.insertTail(0);
					}
			std::cout<<"Ending Program Calc";
			dividend = largeInt("");
		}
	}

	largeInt intResult = largeInt(result,isNegLocal,2);
	intResult.setRadix(originalThisRadix);
	return intResult;
}

largeInt largeInt::operator%(largeInt divisor)
{
	short originalDivisorRadix = divisor.radix,
			originalThisRadix = this->radix;

	bool isNegLocal = false;

	if(divisor.negative)
		isNegLocal = !isNegLocal;
	if(this->negative)
		isNegLocal = !isNegLocal;

	largeInt portionOfDividend("");
	portionOfDividend.setRadix(2);
	largeInt dividend = *this;
	iterator<short> divIt = dividend.number.begin();
	unsortedList<short> result;
	int insertCount = 0;
	divisor.setRadix(2);
	dividend.setRadix(2);
	std::cout<<"PreRun ";
	std::cout<<"Dividend ";
	dividend.printNum();
	std::cout<<"divisor ";
	divisor.printNum();

	while(!dividend.number.isEmpty() && insertCount<20)
	{
		insertCount++;
		std::cout<<std::endl<<"is PortDividend < Divisor: "<<(portionOfDividend < divisor);
		std::cout<<std::endl;

		while(divIt != nullptr && portionOfDividend < divisor)
		{
			if(!portionOfDividend.isZero() || *divIt != 0)
				portionOfDividend.number.insertTail(*divIt);
			divIt++;
			dividend.number.removeHead();
			result.printList();
			if(insertCount == 0)
				result.insertTail(0);
			else
				insertCount--;
			result.printList();
			std::cout<<"portionDividend len:"<<portionOfDividend.number.getLength()<<" num:";
			portionOfDividend.printNum();

			std::cout<<"divisor ";
			divisor.printNum();
		}
		std::cout<<"portionDividend len:"<<portionOfDividend.number.getLength()<<" num:";
		portionOfDividend.printNum();
		if(!portionOfDividend.isZero() && divIt != nullptr
		)
		{
			result.insertTail(1);
			//if(insertCount != 0)
			//	insertCount--;
			largeInt remainder = portionOfDividend-divisor;
			std::cout<<"remainder "<<portionOfDividend.radix<< " ";
			remainder.printNum();
			portionOfDividend.number.emptyList();

			{
				///trim subtraction result
				iterator<short> it = remainder.number.begin();
				while(it != nullptr && *it == 0)
				{
					it++;
					remainder.number.removeHead();
				}
			}

			//insert remainder in head of dividend
			//walk divIt back one place unless divIt.getLength() <= this->getLength()
			if(!remainder.isZero())
			{
				remainder.printNum();
				iterator<short> remainderIt = remainder.number.rbegin();
				while(remainderIt != nullptr)
				{
					portionOfDividend.number.insertHead(*remainderIt);
					//std::cout<<*remainderIt;
					insertCount++;
					remainderIt--;
				}
				portionOfDividend.trim();
				std::cout<<"\ninserted remainder into dividend\n";
			}

			std::cout<<"dividend hello ";
			dividend.printNum();
		}
		else if(divIt == nullptr)
		{
			if (portionOfDividend < divisor || portionOfDividend.isZero())
					{
						//dividend = largeInt("");
						result.insertTail(0);
					}
			std::cout<<"Ending Program Calc";
			dividend = largeInt("");
		}
	}

	portionOfDividend.setRadix(originalDivisorRadix);
	return portionOfDividend;
}

void largeInt::operator-=(largeInt subtrahend)
{
	bool isNeg = false;
		unsortedList<short> result;
		if(subtrahend.isNegative() && !this->isNegative())
		{
			result = addTwoLists(this->number,subtrahend.number,this->radix);
		}
		else if(!subtrahend.isNegative() && this->isNegative())
		{
			result = addTwoLists(this->number,subtrahend.number,this->radix);
			isNeg = true;
		}
		else if(!subtrahend.isNegative() && !this->isNegative())
		{
			if(subtrahend < *this)
			{
				result = subtractTwoLists(this->number,subtrahend.number,radix);
			}
			else
			{
				result = subtractTwoLists(subtrahend.number,this->number,radix);
				isNeg = true;
			}
		}
		else if(subtrahend.isNegative() && this->isNegative())
		{
			if(subtrahend < *this)
			{
				result = subtractTwoLists(this->number,subtrahend.number,radix);
				isNeg = true;
			}
			else
			{
				result = subtractTwoLists(subtrahend.number,this->number,radix);
			}
		}
		largeInt data(result,isNeg);
		data.trim();
		*this = data;
}
largeInt largeInt::operator-(largeInt subtrahend)
{
	bool isNeg = false;
	unsortedList<short> result;
	if(subtrahend.isNegative() && !this->isNegative())
	{
		result = addTwoLists(this->number,subtrahend.number,this->radix);
	}
	else if(!subtrahend.isNegative() && this->isNegative())
	{
		result = addTwoLists(this->number,subtrahend.number,this->radix);
		isNeg = true;
	}
	else if(!subtrahend.isNegative() && !this->isNegative())
	{
		if(subtrahend < *this)
		{
			result = subtractTwoLists(this->number,subtrahend.number,radix);
		}
		else
		{
			result = subtractTwoLists(subtrahend.number,this->number,radix);
			isNeg = true;
		}
	}
	else if(subtrahend.isNegative() && this->isNegative())
	{
		if(subtrahend < *this)
		{
			result = subtractTwoLists(this->number,subtrahend.number,radix);
			isNeg = true;
		}
		else
		{
			result = subtractTwoLists(subtrahend.number,this->number,radix);
		}
	}
	largeInt data(result,isNeg,this->radix);

	data.trim();
	return data;

}

bool largeInt::operator<(largeInt& o)
{
	bool isLessThan = false;
	if(number.getLength() < o.number.getLength())
	{
		isLessThan = true;
	}
	else if (number.getLength() == o.number.getLength())
	{
		iterator<short> tIt = number.begin();
		iterator<short> oIt = o.number.begin();
		while(!isLessThan && tIt != nullptr && oIt != nullptr)
		{
			if(*tIt++ < *oIt++)
				isLessThan = true;
		}
	}
	return isLessThan;
}
bool largeInt::operator<=(largeInt& o)
{
	bool isLessThan = false;
	if(number.getLength() <= o.number.getLength())
	{
		isLessThan = true;
	}
	else if (number.getLength() == o.number.getLength())
	{
		iterator<short> tIt = number.begin();
		iterator<short> oIt = o.number.begin();
		while(!isLessThan && tIt != nullptr && oIt != nullptr)
		{
			if(*tIt++ <= *oIt++)
				isLessThan = true;
		}
	}
	return isLessThan;
}
void largeInt::printNum()
{
	if(negative)
	{
		std::cout<<'-';
	}
	for(short i : number)
		if(i>9)
		{
			std::cout<<char(i+55);
		}
		else
		std::cout<<i;
	std::cout<<std::endl;
}

bool largeInt::operator==(largeInt& other)
{
	return (other.number == this->number && this->radix == other.radix);
}

void largeInt::operator=(const largeInt& other)
{
	this->negative = other.negative;
	this->number = other.number;
}
void largeInt::operator=(int other)
{
	number.emptyList();
	while(other != 0)
	{
		number.insertHead(other%10);
		other /= 10;
	}
}
largeInt::largeInt(int other)
{
	radix = 10;
	if(other < 0)
		negative = true;
	else
		negative = false;
	number.emptyList();
	while(other != 0)
	{
		number.insertHead(other%10);
		other /= 10;
	}
}

bool largeInt::operator!=(int other)
{
	iterator<short> it = number.rbegin();
	bool equal = true;
	while(other != 0 && equal)
	{
		if(other%10 != *it)
			equal = false;
		other /= 10;
		it--;
	}
	return equal;
}

largeInt::largeInt(unsortedList<short> num,bool isNegative,short radix):negative(isNegative)
{
	this->number = num;
	this->radix = radix;
}
largeInt::largeInt(std::string num):negative(false),radix(10)
{
	for(char c : num)
	{
		if(c == '-')
			negative = true;
		else if(c >= 48 && c <58)
			number.insertTail(c-48);
	}

}
