#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cstdlib>
#include <algorithm>
#include <time.h>

using namespace std;
int wins, flag;

//Игрок
struct Player {
	string name;
	string country;
	int year;
	int wins;
	Player(const string& country = "", const string& name = "", int year = 0, int wins = 0) :
		country(country), name(name), year(year), wins(wins) {}
	friend istream& operator >> (istream& in, Player& p) {
		in >> p.country >> p.name >> p.year >> p.wins;
		return in;
	}
};

//паттерн "Наблюдатель"
class Observer
{
  public:
    virtual void update(int value, ofstream&) = 0;
	virtual Player& get() = 0;
};
 
//Сама игра
class Subject
{
	vector<Observer*> m_views;
	ofstream out;
	int num; // количество игроков
public:
	Subject() { num = 0; } // изначально количество 0
	void attach(Observer *obs)
	{
		m_views.push_back(obs);
		num++; // увеличиваем на 1 каждый раз, как добавляется игрок
	}
	void play(int val)
	{
		out.open(to_string(rand()) + to_string(rand()) + ".txt");
		notify();
		for (int i = 0; i < val; ++i) {
			//Играем
			vector<int> chances; // массив с шансами на победу каждого игрока
			int speed, luck;
			for (int j = 0; j < num; ++j) // проходим по всем игрокам
			{
				speed = rand() % 11; // скорость от 0 до 10
				luck = rand() % 11; // удача от 0 до 10
				chances.push_back(speed*luck); // шансы на победу = скорость*удача
			}
			int maxch = 0; // максимальный шанс на победу
			int winner = 0; // номер победившего игрока
			for (int j = 0; j < num; ++j)
			{
				if (maxch < chances.at(j)) // если максимальный шанс меньше текущего
				{
					maxch = chances.at(j); // то текущий шанс делаем максимальным
						winner = j; // и это победитель
				}
			}
			for (int j = 0; j < num; ++j)
			{
				if (winner == j) // если текущий игрок победил
					(m_views.at(j))->get().wins += 1; // увеличиваем его счетчик побед
				else
					(m_views.at(j))->get().wins += 0; // иначе не увеличиваем
			}
			out << "Матч: " << i + 1 << "\n";
			cout << endl << "Матч: " << i + 1 << endl;
			cout << "В этом матче побеждает ";
			cout << (m_views.at(winner))->get().name << endl;
			cout << "Имя игрока\tВсего побед" << endl;
			notify();
			
		}
	}
	//Оповещаем всех наблюдателей
	void notify()
	{
		if (flag == 0)
			cout << "Соотношение побед до начала матча:" << endl;
			sort(m_views.begin(), m_views.end(), [](Observer* a, Observer* b) {return a->get().wins > b->get().wins; });
			for (int i = 0; i < m_views.size(); ++i) {
				m_views[i]->update(i, out);
				cout << "\n"; out << '\n';
		}
	}
};

//Наблюдаем за всеми игроками
class AllObserver : public Observer
{
	Player player;
public:
	AllObserver(Subject *model, const Player& p) :player(p)
	{
		model->attach(this);
	}
	Player& get() { return player; }
	void update(int position, ofstream& out)
	{
		cout << player.name << "\t\t" << player.wins << "\t";
		out << player.name  << "\t\t" << player.wins << "\t";
	}
};
 
//За кем будем наблюдать
class MainObserver: public Observer
{
	Player player; 
	int last_wins;
  public:
	  MainObserver(Subject *model, const Player& p) : player(p), last_wins(p.wins)
    {
        model->attach(this);
    }
	  Player& get() { return player; }
      void update(int position, ofstream& out)
    {
		 bool is_win = false;
		 if (last_wins != player.wins) {
			 last_wins = player.wins;
			 is_win = true;
			 wins++;
		 }
		 if (flag == 0)
		 {
			 out << player.name << "\t\t" << player.wins;
			 cout << player.name << "\t\t" << player.wins;
			 flag = 1;
		 }
		 else
		 {
			 out << player.name << "\t\t" << player.wins << "\t" << (is_win ? " Победа" : " Проигрыш");
			 cout << player.name << "\t\t" << player.wins << "\t" << (is_win ? " Победа" : " Проигрыш");
		 }

    }
};
 
int main()
{
  setlocale(LC_ALL, "Russian");
  srand(time(NULL));
  Subject subj;
  int cnt;
  ifstream in("input.txt");
  in >> cnt;
  vector<Player> players(cnt);
  //Считываем игроков из файла
  for (auto& p : players) {
	  in >> p;
  }
  cout << "Добавить игроков в базу? (1/0): ";
  char ch;
  cin >> ch;
  if (ch == '1') {
	  cout << "\nВведите количество новых игроков: "; cin >> cnt;
	  for (int i = 0; i < cnt; ++i) {
		  string name, country;
		  int year, wins;
		  cout << "Имя: "; cin >> name;
		  cout << "Страна: "; cin >> country;
		  cout << "Победы: "; cin >> wins;
		  cout << "Год: "; cin >> year;
		  players.push_back(Player{country, name, year, wins});
	  }
  }
  cout << "Игроки:" << endl;
  for (int i = 0; i < players.size(); ++i)
  {
	  cout << players.at(i).name << endl;
  }
  cout << "Введите номер игрока для наблюдения (от 0 до " << players.size() - 1 << ") : ";
  cin >> cnt;
  vector<Observer*> obs;
  for (int i = 0; i < players.size(); ++i) {
	  if (i != cnt) {
		  obs.push_back(new AllObserver(&subj, players[i]));
	  }
	  else {
		  obs.push_back(new MainObserver(&subj, players[i]));
	  }
  }
  wins = 0;
  flag = 0;
  subj.play(5);
  if (wins > 2)
	  cout << "Вы выиграли!" << endl;
  else
	  cout << "Вы проиграли!" << endl;

  system("pause>>void");
}