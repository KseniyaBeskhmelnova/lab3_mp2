#pragma once

enum class TypeLexeme {     // ��� �������
	no_type,
	number,
	bin_op,
	unar_op,
	function,
	left_bracket,
	right_bracket,
};

enum class Priority {       // ���������
	no_priority,
	low,
	middle,
	high,
	bracket
};

enum class StatusLexeme {      // ������ �������
	start,
	end,
	error,
	number,
	bin_op,
	unar_op,
	function,
	left_bracket,
	right_bracket,
};

std::vector<std::string> functions = {"abs", "sqrt", "sin", "cos", "tg", "ctg", "ln", "exp"};   // ���������� �������

class Lexeme {
public:
	std::string name;        // �������� �������
	TypeLexeme type;         // ���
	double value = 0.;            // �������� (� ������ �����)
	Priority priority;       // ���������
public:
	Lexeme() :type(TypeLexeme::no_type), name(), priority(Priority::no_priority) {}
	Lexeme(std::string name_, TypeLexeme type_, double value_, Priority priority_) : name(name_), type(type_), value(value_), priority(priority_) {}
	~Lexeme() {}
};