#include<stdlib.h>
#include<iostream>
#include<time.h>
using namespace std;
class Card{
public:
	bool isShow(){return show;}
	int getRank(){return rank;}
	int getSuit(){return suit;}
	void setShow(bool s){show=s;}
	void setRank(int r){rank=r;}
	void setSuit(int s){suit=s;}
private:
	bool show;
	int rank;
	int suit;//0到3分别是黑桃，红桃，梅花，方片。
};
class Pile{
public:
	Pile();
	Pile(int s, int c, bool *whole,int &used);
	~Pile(){delete [] card;}
	void PrintPile();
	int getShow(){return nshow;}
	int getConceal(){return nconceal;}
	void setShow(int s){nshow=s;}
	void setConceal(int c){nconceal=c;}
	Card* getCard(){return card;}
	void trasmit(int i,Pile * pile);
private:
	void add(Card *c,int index,bool show,int rank, int suit);//在牌堆中添加一张正面或反面牌。
	int nshow;//正面表示的牌数
	int nconceal;//反面表示的牌数
	Card *card;//指向牌堆
};
class PileLeft{
public:
	PileLeft();
	PileLeft(int n, bool *whole,int &used);
	~PileLeft(){delete [] card;}
	void PrintPile();
	int getNum(){return num;}
	Card* getCard(){return card;}
	Card flipOne();
	void putOne(Card c);
private:
	void add(Card *c,int index,bool show,int rank, int suit);//在牌堆中添加一张正面或反面牌。
	int num;//左牌堆剩余牌数
	Card *card;//指向牌堆
};
class PileRight{
public:
	PileRight();
	~PileRight(){delete [] card;}
	int getNum(){return num;}
	Card* getCard(){return card;}
	Card fetchOne();
	void putOne(Card c);
private:
	int num;
	Card * card;
};
Pile::Pile()
{
	nshow=0;
	nconceal=0;
	card=new Card[50];
}
Pile::Pile(int s,int c, bool *whole, int & used){
	nshow=s;
	nconceal=c;
	card=new Card[50];
	int temp;
	int index=-1;
	for(int i=0;i<c+s;i++){
		temp=rand()%(52-used);
		for(int j=0;j<52;j++){
			if(!whole[j])
				index++;
			if(index==temp){
				index=j;
				break;
			}
		}
		whole[index]=true;
		used++;
		if(i<c)
			add(card,i,false,index%13+1,index/13);
		else
			add(card,i,true,index%13+1,index/13);
		//cout<<index%13+1<<' '<<index/13<<endl;
		index=-1;
	}
}

void Pile::add(Card *c,int index,bool show,int rank,int suit){
	c[index].setShow(show);
	c[index].setRank(rank);
	c[index].setSuit(suit);
}
void Pile::PrintPile(){
	for(int i=0;i<nconceal+nshow;i++){
		switch(card[i].getSuit()){
			case 0:
				cout<<"黑桃";
				break;
			case 1:
				cout<<"红桃";
				break;
			case 2:
				cout<<"梅花";
				break;
			case 3:
				cout<<"方片";
				break;
		}
		if(card[i].getRank()<11)
			cout<<card[i].getRank()<<' ';
		else{
			if(card[i].getRank()==11)
				cout<<"J ";
			if(card[i].getRank()==12)
				cout<<"Q ";
			if(card[i].getRank()==13)
				cout<<"K ";
		}
		if(card[i].isShow())
			cout<<"正面\;";
		else
			cout<<"反面\;";
	}
	cout<<endl;
}
void Pile::trasmit(int i, Pile * pile){
	//将本牌堆从i开始一直到顶的所有牌传给另一个牌堆，并且按相同顺序
	int k=0;
	int sum=pile->getConceal()+pile->getShow();
	for(;k<nconceal+nshow-i;k++){
		pile->getCard()[sum+k].setRank(card[i+k].getRank());
		pile->getCard()[sum+k].setSuit(card[i+k].getSuit());
		pile->getCard()[sum+k].setShow(card[i+k].isShow());
	}
	nshow-=k;
	pile->setShow(pile->getShow()+k);
}
void PileLeft::PrintPile(){
	for(int i=0;i<num;i++){
		switch(card[i].getSuit()){
			case 0:
				cout<<"黑桃";
				break;
			case 1:
				cout<<"红桃";
				break;
			case 2:
				cout<<"梅花";
				break;
			case 3:
				cout<<"方片";
				break;
		}
		if(card[i].getRank()<11)
			cout<<card[i].getRank()<<' ';
		else{
			if(card[i].getRank()==11)
				cout<<"J ";
			if(card[i].getRank()==12)
				cout<<"Q ";
			if(card[i].getRank()==13)
				cout<<"K ";
		}
		if(card[i].isShow())
			cout<<"正面\;";
		else
			cout<<"反面\;";
	}
	cout<<endl;
}
PileLeft::PileLeft(){
	num=0;
	card=new Card[24];
}
PileLeft::PileLeft(int n, bool *whole,int &used){
	num=n;
	card=new Card[num];
	int temp;
	int index=-1;
	for(int i=0;i<num;i++){
		temp=rand()%(52-used);
		for(int j=0;j<52;j++){
			if(!whole[j])
				index++;
			if(index==temp){
				index=j;
				break;
			}
		}
		whole[index]=true;
		used++;	
		add(card,i,false,index%13+1,index/13);
		index=-1;
	}
}
void PileLeft::add(Card *c,int index,bool show,int rank,int suit){
	c[index].setShow(show);
	c[index].setRank(rank);
	c[index].setSuit(suit);
}

Card PileLeft::flipOne(){
	//返回左牌堆最顶上的暗牌
	num--;
	return card[num];
}

void PileLeft::putOne(Card c){
	//将一张牌放在左牌堆翻开堆的顶
	card[num].setRank(c.getRank());
	card[num].setSuit(c.getSuit());
	card[num].setShow(!c.isShow());
	num++;
}

Card PileRight::fetchOne(){
	//返回左牌堆最顶上的暗牌
	num--;
	return card[num];
}

void PileRight::putOne(Card c){
	//将一张牌放在左牌堆翻开堆的顶
	card[num].setRank(c.getRank());
	card[num].setSuit(c.getSuit());
	card[num].setShow(c.isShow());
	num++;
}
PileRight::PileRight(){
	num=0;
	card=new Card[13];
}