/*===================================================================
	 The all fnctions for convertion between the PU
	       **********INTEL TURBO C*******
=====================================================================*/
#include <stdio.h>
/*-------------------------------------------------------------------*/
#define LENTP16  2           /* the length in  bytes =  2 */
#define LENTP32  4           /* the length in  bytes  = 4 */
#define LENTP64  8           /* the length in  bytes =  8 */

#define NUMCOMP  4     /* the number of computers */

/*----------The new types--------------------------------------------*/
   typedef  int   INT32;   /**** long for MS DOS TC ****/

/*-------------------------------------------------------------------*/

 struct ordbyte16 {
       int  nc;                       /* the number of order byte  in computer */
       int  bp[LENTP16];     /* the byte's place in RAM */
		  };
 struct ordbyte32 {
       int  nc;                       /* the number of order byte  in computer */
       int  bp[LENTP32];     /* the byte's place in RAM */
		   };
 struct ordbyte64 {
       int  nc;                       /* the number of order byte  in computer */
       int  bp[LENTP64];     /* the byte's place in RAM */
		   };

 struct present_t16 {
      unsigned char  byte[LENTP16];  /* the represantation type */
		    };
 struct present_t32 {
      unsigned char  byte[LENTP32];  /* the represantation type */
		     };
 struct present_t64 {
      unsigned char  byte[LENTP64];  /* the represantation type */
		     };

  typedef   struct present_t16  trans16_t;
  typedef   struct present_t32  trans32_t;
  typedef   struct present_t64  trans64_t;


/*-----------The prototype description of functions-----------------*/

   short   i16rm( short );   short   i16ri( short );
   short   i16im( short );    short   i16ir( short );
   short   i16mi( short );    short   i16mr( short );

/***   int  i32rm( int );      int  i32ri( int );
   INT32  i32im( INT32 );  INT32  i32ir( INT32 );
   int  i32mi( int );      int  i32mr( int );
***/

   INT32  i32rm( INT32 );  INT32  i32ri( INT32 );
   INT32  i32im( INT32 );  INT32  i32ir( INT32 );
   INT32  i32mi( INT32 );  INT32  i32mr( INT32 );

   float  f32rm( float );    float  f32ri( float );
   float  f32im( float );     float  f32ir( float );
   float  f32mi( float );     float  f32mr( float );

   double  f64rm( double );   double  f64ri( double );
   double  f64im( double );    double  f64ir( double );
   double  f64mi( double );    double  f64mr( double );

			  /* - - - - - - - - - - - - - - */
 trans16_t  *pconvt16( int, int ,  trans16_t *);    /* return the pointer for need new type */
 trans32_t  *pconvt32( int, int ,  trans32_t *);    /* return the pointer for need new type */
 trans64_t  *pconvt64( int, int ,  trans64_t *);    /* return the pointer for need new type */

/*===================================================================
		  Function FOR SHORT 16
=====================================================================*/

/*------------------Risc to Motorolla (short16)----------------------*/
  short  i16rm( short  idelv )
  {
      short     *pis;

     pis= ( short *) pconvt16( 2, 1, (trans16_t *) &idelv );
     return( *pis );
  }
/*-------------------- Motorolla to Risc (short16)-------------------*/
  short  i16mr( short  idelv )
  {
     short   *pff;

      pff = ( short *) pconvt16( 1, 2, (trans16_t *)  &idelv);
     return( *pff );
  }
/*-----------------Risc to Intell (short16)---------------------------*/
  short  i16ri( short  idelv )
  {
      short   *pff;

     pff = & idelv;
     /**  pff = ( short *) pconvt16( 2, 3,  (trans16_t *)  &idelv); **/
     return( *pff );
  }
 /*-----------------Intell to Risc (short16)----------------------*/
   short  i16ir( short  idelv )
  {
      short   *pff;

      pff = & idelv;
     /**   pff = ( short *) pconvt16( 3, 2,  (trans16_t *)  &idelv); **/
     return( *pff );
  }
 /*------------------Motorolla to Intell (short 16)--------------*/
  short  i16mi( short  idelv )
  {
      short   *pff;

      pff = ( short *) pconvt16( 1, 3,  (trans16_t *)  &idelv);
     return( *pff );
  }
 /*------------------Intell to Motorolla  (short 16)---------*/
  short  i16im( short  idelv )
  {
      short   *pff;

      pff = ( short *) pconvt16( 3, 1,  (trans16_t *)  &idelv);
     return( *pff );
  }
/*===================================================================
		   Functions  FOR   INTEGER 32
=====================================================================*/

/*--------------------------Risc to Motorolla (int32)----------------*/
  INT32  i32rm( INT32  idelv )
  {
      INT32   *pff;

      pff = ( INT32 *) pconvt32( 2, 1, (trans32_t *)  &idelv);
     return( *pff );
  }
/*--------------------Motorolla  to Risc (int32)----------------------*/
  INT32  i32mr( INT32  idelv )
  {
      INT32   *pff;

      pff = ( INT32 *) pconvt32( 1, 2, (trans32_t *)  &idelv);
     return( *pff );
  }
/*----------------Risc to Intell (int32)-----------------------------*/
 INT32   i32ri( INT32 idelv )
  {
      INT32   *pff;

     pff = & idelv;
      /**  pff = ( INT32 *) pconvt32( 2, 3,  (trans32_t *)  &idelv); **/
     return( *pff );
  }
/*----------------Intell  to Risc (int32)--------------------------*/
  INT32 i32ir( INT32 idelv )
  {
      INT32   *pff;

     pff = & idelv;
      /**  pff = ( INT32 *) pconvt32(  3,  2,  (trans32_t *)  &idelv); **/
     return( *pff );
  }
/*------------------Motorolla to Intell (int32)-----------*/
  INT32  i32mi( INT32 idelv )
   {
      INT32   *pff;

      pff = ( INT32 *) pconvt32( 1,  3,  (trans32_t *)  &idelv);
     return( *pff );
  }
/*--------------------Intell to Motorolla (int32)------------*/
   INT32  i32im( INT32 idelv )
   {
      INT32   *pff;

      pff = ( INT32 *) pconvt32( 3, 1,  (trans32_t *)  &idelv);
     return( *pff );
   }

/*===================================================================
		   Function   FOR   FLOAT   32
====================================================================*/

/*-----------------Risc to Motorolla ( float32)---------------------*/
  float  f32rm( float  fdelv )
  {
      float   *pff;

      pff = ( float *) pconvt32( 2, 1, (trans32_t *)  &fdelv);
     return( *pff );
  }
/*----------------- Motorolla  to Risc( float32)--------------------*/
  float  f32mr( float  fdelv )
  {
      float   *pff;

      pff = ( float *) pconvt32( 1, 2, (trans32_t *)  &fdelv);
     return( *pff );
  }
/*-----------------------Intell to Risc ( float 32 )-----------------*/
   float  f32ir( float  fdelv )
  {
      float   *pff;

      pff = ( float *) pconvt32( 3, 2, (trans32_t *)  &fdelv);
     return( *pff );
  }
/*-----------------------Risc to Intell ( float 32 )-----------------*/
   float  f32ri( float  fdelv )
  {
      float   *pff;

      pff = ( float *) pconvt32( 2, 3, (trans32_t *)  &fdelv);
     return( *pff );
  }
/*-----------------------Motorolla to Intell ( float 32 )--------------*/
   float  f32mi( float  fdelv )
  {
      float   *pff;

      pff = ( float *) pconvt32( 1, 3, (trans32_t *)  &fdelv);
     return( *pff );
  }
/*-----------------------Intell to Motorolla ( float 32 )--------------*/
   float  f32im( float  fdelv )
  {
      float   *pff;

      pff = ( float *) pconvt32( 3, 1, (trans32_t *)  &fdelv);
     return( *pff );
  }
/*===================================================================
			  Function   FOR  DOUBLE  64
====================================================================*/

/*------------------Risc to Motorolla (double64)--------------------*/
   double  f64rm( double  fdelv )
  {
      double   *pff;

      pff = ( double *) pconvt64( 2, 1, (trans64_t *)  &fdelv);
     return( *pff );
  }
/*------------------Motorolla to Risc (double64)--------------------*/
   double  f64mr( double  fdelv )
  {
      double   *pff;

      pff = ( double *) pconvt64( 1, 2, (trans64_t *)  &fdelv);
     return( *pff );
  }
/*------------------Motorolla to Intell (double64)--------------------*/
   double  f64mi( double  fdelv )
  {
      double   *pff;

      pff = ( double *) pconvt64( 1, 3, (trans64_t *)  &fdelv);
     return( *pff );
  }
/*------------------Intell to Motorolla  (double64)--------------------*/
   double  f64im( double  fdelv )
  {
      double   *pff;

      pff = ( double *) pconvt64( 3, 1, (trans64_t *)  &fdelv);
     return( *pff );
  }
 /*------------------ Risc to Intell(double64)--------------------*/
   double  f64ri( double  fdelv )
  {
      double   *pff;

      pff = ( double *) pconvt64( 2, 3,  (trans64_t *)  &fdelv);
     return( *pff );
  }
/*------------------Intell to Risc (double64)--------------------*/
   double  f64ir( double  fdelv )
  {
      double   *pff;

      pff = ( double *) pconvt64( 3, 2, (trans64_t *)  &fdelv);
     return( *pff );
  }

/*===================================================================
	      Internal functions for converts
===================================================================*/

/*------------Transformation  from delv to recv type  pdelv------*/
  trans16_t  *pconvt16(  int  delv, int recv, trans16_t *ptp)
  {

     static trans16_t  ntype,  *pdelv;
     int  idelv=0, irecv=0;
     /*int  ialloc=0;*/
		       /* The table of represantation bytes in  type */
       static  struct ordbyte16 c[NUMCOMP]=
				 {
		 { 0, { 0, 1 } },    /* Dumb */
		 { 1, { 0, 1 } },   /* Motorolla -- 1*/
		 { 2, { 1, 0 } },    /* Risc -- 2*/
		 { 3, { 1, 0 } }     /* Intel -- 3 */
				 };

     pdelv= ptp;

     while( idelv < LENTP16 )
	      {
	irecv=0;
	while( (c[recv].bp)[irecv] != (c[delv].bp)[idelv]  )  irecv++;
	 ntype.byte[irecv]=pdelv->byte[idelv];
	 idelv++;
	      }
     return ( &ntype );
  }
/*--------------Transformation  from delv to recv type  pdelv------*/
  trans32_t  *pconvt32(  int  delv, int recv, trans32_t *ptp)
  {

     static trans32_t  ntype,  *pdelv;
     int  idelv=0, irecv=0;
     /*int  ialloc=0;*/
		       /* The table of represantation bytes in  type */
       static  struct ordbyte32 c[NUMCOMP]=
				 {
		 { 0, { 0, 1, 2, 3 } },    /* Dumb */
		 { 1, { 0, 1, 2, 3 } },   /* Motorolla --1 */
		 { 2, { 3, 2, 1, 0 } },    /* Risc -- 2 */
		 { 3, { 3, 2, 1, 0 } }    /* Intel --3 */
				};

     pdelv= ptp;

     while( idelv < LENTP32 )
	      {
	irecv=0;
	while( (c[recv].bp)[irecv] != (c[delv].bp)[idelv]  )  irecv++;
	 ntype.byte[irecv]=pdelv->byte[idelv];
	 idelv++;
	      }
     return ( &ntype );
  }
/*--------------Transformation  from delv to recv type  pdelv--------*/
  trans64_t  *pconvt64(  int  delv, int recv, trans64_t *ptp)
  {

     static trans64_t  ntype,  *pdelv;
     int  idelv=0, irecv=0;
     /*int  ialloc=0;*/
		       /* The table of represantation bytes in  type */
       static  struct ordbyte64 c[NUMCOMP]=
				 {
		 { 0, { 0, 1, 2, 3, 4, 5, 6, 7 } },    /* Dumb */
		 { 1, { 0, 1, 2, 3, 4, 5, 6, 7 } },   /* Motorolla --1 */
		 { 2, { 7, 6, 5, 4, 3, 2, 1, 0 } },    /* Risc -- 2  */
		 { 3, { 7, 6, 5, 4, 3, 2, 1, 0 } }    /* Intel -- 3 */
				 };

     pdelv= ptp;

     while( idelv < LENTP64 )
	      {
	irecv=0;
	while( (c[recv].bp)[irecv] != (c[delv].bp)[idelv]  )  irecv++;
	 ntype.byte[irecv]=pdelv->byte[idelv];
	 idelv++;
	      }
     return ( &ntype );
  }

/*===================================================================*/
