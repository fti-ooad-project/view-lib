extern "C"
{
	void init( int N );
	void release();
	double calcDet( double const *inmat , int N );
	void testNative();
}