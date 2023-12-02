// lab3.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <vector>
#include <string>
#include <stack>
#include "lab3.hpp"


// Получение типа лексемы 
TypeLexeme GetType(char symbol, StatusLexeme status) {  
	if (symbol >= '0' && symbol <= '9')
		return TypeLexeme::number;
	else if (symbol == '-' && (status == StatusLexeme::start || status == StatusLexeme::left_bracket))
		return TypeLexeme::unar_op;
	else if (symbol == '+' || symbol == '-' || symbol == '*' || symbol == '/' || symbol == '^')
		return TypeLexeme::bin_op;
	else if (symbol >= 'a' && symbol <= 'z')
		return TypeLexeme::function;
	else if (symbol == '(')
		return TypeLexeme::left_bracket;
	else if (symbol == ')')
		return TypeLexeme::right_bracket;
	throw "Error! Invalid character! (GetType)";
}


// Получение следующей лексемы
Lexeme GetNextLexeme(std::string input, int &index, StatusLexeme status) {
	char symbol = input[index];
	if (symbol >= '0' && symbol <= '9') {
		char* ptrEnd;
		double value = strtod(&input[index], &ptrEnd);
		int dif = (static_cast<const char*>(ptrEnd) - &input[index]);
		std::string name = input.substr(index, dif);
		index += dif - 1;
		return Lexeme(name, TypeLexeme::number, value, Priority::no_priority);
	}
	else if (symbol == '-' && (status == StatusLexeme::start || status == StatusLexeme::left_bracket))
		return Lexeme(std::string(1, symbol), TypeLexeme::unar_op, 0, Priority::high);
	else if (symbol == '-' || symbol == '+')
		return Lexeme(std::string(1, symbol), TypeLexeme::bin_op, 0, Priority::low);
	else if (symbol == '*' || symbol == '/')
		return Lexeme(std::string(1, symbol), TypeLexeme::bin_op, 0, Priority::middle);
	else if (symbol == '^')
		return Lexeme(std::string(1, symbol), TypeLexeme::bin_op, 0, Priority::high);
	else if (symbol >= 'a' && symbol <= 'z') {
		int i = index;
		for (i; input[i] != '('; i++) {}
		std::string name = input.substr(index, i - index);
		index = i-1;
		return Lexeme(name, TypeLexeme::function, 0, Priority::high);
	}
	else if (symbol == '(')
		return Lexeme(std::string(1, symbol), TypeLexeme::left_bracket, 0, Priority::bracket);
	else if (symbol == ')')
		return Lexeme(std::string(1, symbol), TypeLexeme::right_bracket, 0, Priority::bracket);
	throw "Error! Invalid character! (GetNextLexeme)";
}


// Разбиение строки на лексемы
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
				else if (curr_type == TypeLexeme::unar_op)
					status = StatusLexeme::unar_op;
				else if (curr_type == TypeLexeme::function)
					status = StatusLexeme::function;
				else if (curr_type == TypeLexeme::left_bracket) {
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
				else if (curr_type == TypeLexeme::right_bracket) {
					status = StatusLexeme::right_bracket;
					rightBracketCounter++;
				}
				else if (i == input.size() - 1 && leftBracketCounter == rightBracketCounter)
					status = StatusLexeme::end;
				else throw "Incorrect input!";
			}
			else if (status == StatusLexeme::unar_op || status == StatusLexeme::bin_op) {
				if (curr_type == TypeLexeme::number)
					status = StatusLexeme::number;
				else if (curr_type == TypeLexeme::function)
					status = StatusLexeme::function;
				else if (curr_type == TypeLexeme::left_bracket) {
					status = StatusLexeme::left_bracket;
					leftBracketCounter++;
				}
				else throw "Incorrect input!";
			}
			else if (status == StatusLexeme::function) {
				if (curr_type == TypeLexeme::left_bracket) {
					status = StatusLexeme::left_bracket;
					leftBracketCounter++;
				}
				else throw "Incorrect input!";
			}
			res.push_back(curr_lex);
		}
	}
	if (leftBracketCounter!=rightBracketCounter)
		throw "Incorrect input! Check brackets!";
	return res;
}


// Перевод в постфиксную форму
std::vector<Lexeme> ToPostfix(std::vector<Lexeme> input) {
	std::stack<Lexeme> stack;
	std::vector<Lexeme> res;
	for (Lexeme item : input) {
		if (item.type == TypeLexeme::number)
			res.push_back(item);
		else if (item.type == TypeLexeme::unar_op || item.type == TypeLexeme::left_bracket || item.type == TypeLexeme::function)
			stack.push(item);
		else if (item.type == TypeLexeme::right_bracket) {
			while (stack.top().type != TypeLexeme::left_bracket) {
				res.push_back(stack.top());
				stack.pop();
			}
			stack.pop();
		}
		else if (item.type == TypeLexeme::bin_op) {
			while (!stack.empty()) {
				if (item.priority <= stack.top().priority && stack.top().type != TypeLexeme::left_bracket) {
					res.push_back(stack.top());
					stack.pop();
				}
				else
					break;
			}
			stack.push(item);
		}
	}
	while (!stack.empty()) {
		res.push_back(stack.top());
		stack.pop();
	}
	return res;
}


// Вычисление в постфиксной форме
double Calculate(std::vector<Lexeme> postfix) {
	std::stack<Lexeme> stack;
	for (Lexeme item : postfix) {
		if (item.type == TypeLexeme::number)
			stack.push(item);
		else if (item.type == TypeLexeme::bin_op) {
			double a = stack.top().value;
			stack.pop();
			double b = stack.top().value;
			stack.pop();
			double res;
			if (item.name == "+")
				res = a + b;
			else if (item.name == "-")
				res = b - a;
			else if (item.name == "*")
				res = a * b;
			else if (item.name == "/") {
				if (a == 0)
					throw "Division by 0!";
				else
					res = b / a;
			}
			else if (item.name == "^")
				res = pow(b, a);
			stack.push({std::to_string(res), TypeLexeme::number, res, Priority::no_priority});
		}
		else if (item.type == TypeLexeme::unar_op) {
			double a = stack.top().value;
			stack.pop();
			stack.push({ std::to_string(-a), TypeLexeme::number, (-a), Priority::no_priority });

		}
		else if (item.type == TypeLexeme::function) {
			double a = stack.top().value;
			double res;
			stack.pop();
			if (item.name == "abs")
				res = abs(a);
			else if (item.name == "sqrt")
				res = sqrt(a);
			else if (item.name == "sin")
				res = sin(a);
			else if (item.name == "cos")
				res = cos(a);
			else if (item.name == "tg")
				res = tan(a);
			else if (item.name == "ctg")
				res = 1/(tan(a));
			else if (item.name == "ln")
				res = log(a);
			else if (item.name == "exp")
				res = exp(a);
			stack.push({ std::to_string(res), TypeLexeme::function, res, Priority::high });
		}
	}
	return stack.top().value;
}


int main()
{
	std::cout << "Enter arinhmetic expression: ";
	std::string input;
	std::cin >> input;
	std::vector<Lexeme> parsed = Parse(input);
	std::vector<Lexeme> postfix = ToPostfix(parsed);
	double res = Calculate(postfix);
	std::cout << "Result: " << res;
}
