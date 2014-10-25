#include <iostream>
#include <string>
#include <stack>
#include <algorithm>
using namespace std;

int expressionHandler(string exp, char x)
{
	stack<pair<int, int>> expStack;
	for (int i = 0; i < exp.length(); ++i)
	{
		pair<int, int> first, second, result;
		switch (exp[i])
		{
		case '1':
			expStack.push(make_pair(0, 0));
			break;
		case '.':
			if (expStack.size() < 2)
				throw exception("Operation '.' : Too few arguments!");
			second = expStack.top();
			expStack.pop();
			first = expStack.top();
			expStack.pop();
			result = make_pair(INT_MIN, 0);
			if (first.first >= 0 && second.first >= 0)
			{
				if (first.first == INT_MAX && second.first == INT_MAX)
					result.first = INT_MAX;
				else
					result.first = first.first + second.first;
			}
			if (second.first == INT_MAX || (first.first == INT_MAX && second.first > 0))
				result.second = INT_MAX;
			else if (second.first >= 0)
				result.second = max(first.second + second.first, second.second);
			else
				result.second = second.second;
			expStack.push(result);
			break;
		case '+':
			if (expStack.size() < 2)
				throw exception("Operation '+' : Too few arguments!");
			second = expStack.top();
			expStack.pop();
			first = expStack.top();
			expStack.pop();
			result = make_pair(max(first.first, second.first), max(first.second, second.second));
			expStack.push(result);
			break;
		case '*':
			if (expStack.size() < 1)
				throw exception("Operation '*' : Too few arguments!");
			first = expStack.top();
			expStack.pop();
			result = make_pair(0, first.second);
			if (first.first > 0)
				result.first = result.second = INT_MAX;
			expStack.push(result);
			break;
		default:
			if (exp[i] != 'a' && exp[i] != 'b' && exp[i] != 'c')
				throw exception("Incorrect symbol!");
			if (exp[i] == x)
				expStack.push(make_pair(1, 1));
			else
				expStack.push(make_pair(INT_MIN, 0));
		}
	}
	if (expStack.size() != 1)
		throw exception("Incorrect expression!");
	return expStack.top().second;
}

int main()
{
	string exp;
	char x;
	cin >> exp >> x;
	int ans;
	try
	{
		ans = expressionHandler(exp, x);
	}
	catch (exception ex)
	{
		cout << ex.what();
		return 1;
	}
	if (ans == INT_MAX)
		cout << "INF";
	else
		cout << ans;
	cout << endl;
	return 0;
}