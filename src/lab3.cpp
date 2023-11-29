// lab3.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <vector>
#include <stack>
#include "lab3.hpp"
//#include <map>
//using namespace std;

//class TAriphmeticExpression {
//	string infix;
//	string postfix;
//	vector<char> lexems;
//	map<char, int> priority;
//	map<char, double> operands;
//	void Parse();
//	void ToPostfix();
//public:
//	TAriphmeticExpression(string infx);
//	string GetInfix() const {
//		return infix;
//	}
//	string GetPostfix() const {
//		return postfix;
//	}
//	vector<char> GetOperands() const;
//	double Calculate(const map<char, double>& values);
//};
//
//TAriphmeticExpression::TAriphmeticExpression(string infx) : infix(infx) {
//	priority = { {'+' , 1}, {'-' , 1}, {'*', 2}, {'/', 2} };
//	ToPostfix();
//};
//
//vector<char> TAriphmeticExpression::GetOperands() const {
//	vector<char> op;
//	for (const auto& item : operands)
//		op.push_back(item.first);
//	return op;
//}
//
//void TAriphmeticExpression::Parse() {
//
//}
//
//void TAriphmeticExpression::ToPostfix() {
//
//}
//
//double TAriphmeticExpression::Calculate() {
//
//}


enum class TypeLexeme {
	no_type,
	number,
	unar_op,
	bin_op,
	left_bracket,
	right_bracket,
};

enum class Priority {
	no_priority,
	high,
	middle,
	low,
	bracket
};

enum class StatusLexeme {
	start,
	end,
	error,
	number,
	bin_op,
	unar_op,
	left_bracket,
	right_bracket,
};

class Lexeme {
public:
	std::string name;
	TypeLexeme type;
	double value;
	Priority priority;
public:
	Lexeme():type(TypeLexeme::no_type), name(), priority(Priority::no_priority){}
	Lexeme(std::string name_, TypeLexeme type_, double value_, Priority priority_): name(name_), type(type_), value(value_), priority(priority_){}
	~Lexeme(){}
};

TypeLexeme GetType(char symbol, StatusLexeme status) {
	if (symbol >= '0' && symbol <= '9')
		return TypeLexeme::number;
	else if (symbol == '-' && (status == StatusLexeme::start || status == StatusLexeme::left_bracket))
		return TypeLexeme::unar_op;
	else if ((symbol == '+' || symbol == '-' || symbol == '*' || symbol == '/'))// && (status != StatusLexeme::start && status != StatusLexeme::left_bracket))
		return TypeLexeme::bin_op;
	else if (symbol == '(')
		return TypeLexeme::left_bracket;
	else if (symbol == ')')
		return TypeLexeme::right_bracket;
	throw "Error! Invalid character! (GetType)";
}

Lexeme GetNextLexeme(std::string input, int &index, StatusLexeme status) {
	char symbol = input[index];
	if (symbol >= '0' && symbol <= '9') {
		char* ptrEnd;
		double value = strtod(&input[index], &ptrEnd);
		int dif = (static_cast<const char*>(ptrEnd) - &input[index]);
		std::string name = input.substr(index, dif);
		index += dif;
		return Lexeme(name, TypeLexeme::number, value, Priority::no_priority);
	}
	else if (symbol == '-' && (status == StatusLexeme::start || status == StatusLexeme::left_bracket))
		return Lexeme(std::string(1, symbol), TypeLexeme::unar_op, 0, Priority::high);
	else if (symbol == '-' || symbol == '+')
		return Lexeme(std::string(1, symbol), TypeLexeme::bin_op, 0, Priority::low);
	else if (symbol == '*' || symbol == '/')
		return Lexeme(std::string(1, symbol), TypeLexeme::bin_op, 0, Priority::middle);
	else if (symbol == '(')
		return Lexeme(std::string(1, symbol), TypeLexeme::left_bracket, 0, Priority::bracket);
	else if (symbol == ')')
		return Lexeme(std::string(1, symbol), TypeLexeme::right_bracket, 0, Priority::bracket);
	throw "Error! Invalid character! (GetNextLexeme)";
}

std::vector<Lexeme> Parse(std::string input) {
	std::vector<Lexeme> res;
	StatusLexeme status = StatusLexeme::start;
	int leftBracketCounter = 0, rightBracketCounter = 0;
	for (int i = 0; i < input.size(); i++) {
		if (rightBracketCounter > leftBracketCounter)
			throw "Incorrect input! Check brackets!";
		else {
			Lexeme curr_lex = GetNextLexeme(input, i, status);
			TypeLexeme curr_type = curr_lex.type;
			if (status == StatusLexeme::start || status == StatusLexeme::left_bracket) {
				if (curr_type == TypeLexeme::number)
					status = StatusLexeme::number;
				else if (status == StatusLexeme::unar_op)
					status = StatusLexeme::unar_op;
				else if (status == StatusLexeme::left_bracket) {
					status = StatusLexeme::left_bracket;
					leftBracketCounter++;
				}
				else if (status == StatusLexeme::start && i == input.size()-1)
					throw "Nothing entered!";
				else throw "Incorrect input!";
			}
			else if (status == StatusLexeme::number || status == StatusLexeme::right_bracket) {
				if (curr_type == TypeLexeme::bin_op)
					status = StatusLexeme::bin_op;
				if (curr_type == TypeLexeme::right_bracket) {
					status = StatusLexeme::right_bracket;
					rightBracketCounter++;
				}
				else if (i == input.size() - 1)
					status = StatusLexeme::end;
				else throw "Incorrect input!";
			}
			else if (status == StatusLexeme::unar_op || status == StatusLexeme::bin_op) {
				if (curr_type == TypeLexeme::number)
					status = StatusLexeme::number;
				else if (status == StatusLexeme::left_bracket) {
					status = StatusLexeme::left_bracket;
					leftBracketCounter++;
				}
				else throw "Incorrect input!";
			}
			res.push_back(curr_lex);
		}
	}
	return res;
}

std::vector<Lexeme> ToPostfix(std::vector<Lexeme> input) {
	std::stack<Lexeme> stack;
	std::vector<Lexeme> res;
	for (Lexeme item : input) {
		if (item.type == TypeLexeme::number || item.type == TypeLexeme::unar_op || item.type == TypeLexeme::left_bracket)
			res.push_back(item);
		else if (item.type == TypeLexeme::right_bracket) {
			while (stack.top().type != TypeLexeme::left_bracket) {
				res.push_back(stack.top());
				stack.pop();
			}
			stack.pop();
		}
		else if (item.type == TypeLexeme::bin_op) {
			while (!stack.empty()) {
				Lexeme top = stack.top();
				if (item.priority >= top.priority) {
					res.push_back(top);
					stack.pop();
				}
			}
		}
	}
	while (!stack.empty()) {
		res.push_back(stack.top());
		stack.pop();
	}
	return res;
}

double Calculate(std::vector<Lexeme> postfix) { return 0.; }

int main()
{

}
