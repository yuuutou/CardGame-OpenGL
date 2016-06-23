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
	int suit;//0��3�ֱ��Ǻ��ң����ң�÷������Ƭ��
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
	void add(Card *c,int index,bool show,int rank, int suit);//���ƶ������һ����������ơ�
	int nshow;//�����ʾ������
	int nconceal;//�����ʾ������
	Card *card;//ָ���ƶ�
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
	void add(Card *c,int index,bool show,int rank, int suit);//���ƶ������һ����������ơ�
	int num;//���ƶ�ʣ������
	Card *card;//ָ���ƶ�
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
				cout<<"����";
				break;
			case 1:
				cout<<"����";
				break;
			case 2:
				cout<<"÷��";
				break;
			case 3:
				cout<<"��Ƭ";
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
			cout<<"����\;";
		else
			cout<<"����\;";
	}
	cout<<endl;
}
void Pile::trasmit(int i, Pile * pile){
	//�����ƶѴ�i��ʼһֱ�����������ƴ�����һ���ƶѣ����Ұ���ͬ˳��
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
				cout<<"����";
				break;
			case 1:
				cout<<"����";
				break;
			case 2:
				cout<<"÷��";
				break;
			case 3:
				cout<<"��Ƭ";
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
			cout<<"����\;";
		else
			cout<<"����\;";
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
	//�������ƶ���ϵİ���
	num--;
	return card[num];
}

void PileLeft::putOne(Card c){
	//��һ���Ʒ������ƶѷ����ѵĶ�
	card[num].setRank(c.getRank());
	card[num].setSuit(c.getSuit());
	card[num].setShow(!c.isShow());
	num++;
}

Card PileRight::fetchOne(){
	//�������ƶ���ϵİ���
	num--;
	return card[num];
}

void PileRight::putOne(Card c){
	//��һ���Ʒ������ƶѷ����ѵĶ�
	card[num].setRank(c.getRank());
	card[num].setSuit(c.getSuit());
	card[num].setShow(c.isShow());
	num++;
}
PileRight::PileRight(){
	num=0;
	card=new Card[13];
}