#include "val.h"

using namespace std;

Value Value::operator+(const Value& op) const
{
	if (IsInt() && op.IsInt())
	{
		return Value(GetInt() + op.GetInt());
	}
	else if (IsReal() && op.IsReal())
	{
		return Value(GetReal() + op.GetReal());
	}
	else if (IsInt() && op.IsReal())
	{
		return Value(GetInt() + op.GetReal());
	}
	else if (IsReal() && op.IsInt())
	{
		return Value(GetReal() + op.GetInt());
	}

	return Value();
}

Value Value::operator-(const Value& op) const
{
	if (IsInt() && op.IsInt())
	{
		return Value(GetInt() - op.GetInt());
	}
	else if (IsReal() && op.IsReal())
	{
		return Value(GetReal() - op.GetReal());
	}
	else if (IsInt() && op.IsReal())
	{
		return Value(GetInt() - op.GetReal());
	}
	else if (IsReal() && op.IsInt())
	{
		return Value(GetReal() - op.GetInt());
	}

	return Value();
}

Value Value::operator/(const Value& op) const
{
	if (op.IsInt() && op.GetInt() == 0)
	{
		return Value();
	}
	else if (op.IsReal() && op.GetReal() == 0.0)
	{
		return Value();
	}

	if (IsInt() && op.IsInt())
	{
		return Value(GetInt() / op.GetInt());
	}
	else if (IsReal() && op.IsReal())
	{
		return Value(GetReal() / op.GetReal());
	}
	else if (IsInt() && op.IsReal())
	{
		return Value(GetInt() / op.GetReal());
	}
	else if (IsReal() && op.IsInt())
	{
		return Value(GetReal() / op.GetInt());
	}

	return Value();
}

Value Value::operator*(const Value& op) const
{
	if (IsInt() && op.IsInt())
	{
		return Value(GetInt() * op.GetInt());
	}
	else if (IsReal() && op.IsReal())
	{
		return Value(GetReal() * op.GetReal());
	}
	else if (IsInt() && op.IsReal())
	{
		return Value(GetInt() * op.GetReal());
	}
	else if (IsReal() && op.IsInt())
	{
		return Value(GetReal() * op.GetInt());
	}

	return Value();
}

Value Value::Catenate(const Value& op) const
{
	if (!IsString() || !op.IsString())
	{
		return Value();
	}

	return Value(GetString() + op.GetString());
}

Value Value::Power(const Value& op) const
{
	if (IsInt() && op.IsInt())
	{
		return Value(pow(GetInt(), op.GetInt()));
	}
	else if (IsReal() && op.IsReal())
	{
		return Value(pow(GetReal(), op.GetReal()));
	}
	else if (IsInt() && op.IsReal())
	{
		return Value(pow(GetInt(), op.GetReal()));
	}
	else if (IsReal() && op.IsInt())
	{
		return Value(pow(GetReal(), op.GetInt()));
	}

	return Value();
}

Value Value::operator==(const Value& op) const
{
	if (IsInt() && op.IsInt())
	{
		return Value(GetInt() == op.GetInt());
	}
	else if (IsReal() && op.IsReal())
	{
		return Value(GetReal() == op.GetReal());
	}
	else if (IsString() && op.IsString())
	{
		return Value(GetString() == op.GetString());
	}
	else if (IsBool() && op.IsBool())
	{
		return Value(GetBool() == op.GetBool());
	}
	else if (IsInt() && op.IsReal())
	{
		return Value(GetInt() == op.GetReal());
	}
	else if (IsReal() && op.IsInt())
	{
		return Value(GetReal() == op.GetInt());
	}

	return Value();
}

Value Value::operator>(const Value& op) const
{
	if (IsInt() && op.IsInt())
	{
		return Value(GetInt() > op.GetInt());
	}
	else if (IsReal() && op.IsReal())
	{
		return Value(GetReal() > op.GetReal());
	}
	else if (IsString() && op.IsString())
	{
		return Value(GetstrLen() > op.GetstrLen());
	}
	else if (IsInt() && op.IsReal())
	{
		return Value(GetInt() > op.GetReal());
	}
	else if (IsReal() && op.IsInt())
	{
		return Value(GetReal() > op.GetInt());
	}

	return Value();
}

Value Value::operator<(const Value& op) const
{
	if (IsInt() && op.IsInt())
	{
		return Value(GetInt() < op.GetInt());
	}
	else if (IsReal() && op.IsReal())
	{
		return Value(GetReal() < op.GetReal());
	}
	else if (IsString() && op.IsString())
	{
		return Value(GetstrLen() < op.GetstrLen());
	}
	else if (IsInt() && op.IsReal())
	{
		return Value(GetInt() < op.GetReal());
	}
	else if (IsReal() && op.IsInt())
	{
		return Value(GetReal() < op.GetInt());
	}

	return Value();
}