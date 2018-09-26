long fact();

long main()
{
	return fact(5);
}

long fact()
{
	long arg;
	if(arg < 2)
	{
		return 1;
	};
	return arg * fact(arg - 1);
}