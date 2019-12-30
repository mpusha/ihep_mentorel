static short f_list[3][3]={
    {0,1,2},   /*i to r,i to m*/
    {3,0,4},   /*r to i,r to m*/
    {5,6,0},   /*m to i,m to r*/
   };
preob16(i16,from,to)
  short *i16;
  int from,to;
   {
    switch(f_list[from-1][to-1])
      {
       case 1:
       *i16=i16ir(*i16);
       break;
       case 2:
       *i16=i16im(*i16);
       break;
       case 3:
       *i16=i16ri(*i16);
       break;
       case 4:
       *i16=i16rm(*i16);
       break;
       case 5:
       *i16=i16mi(*i16);
       break;
       case 6:
       *i16=i16mr(*i16);
       break;
      }
   }
preob32(i32,from,to)
  int *i32;
  int from,to;
   {
    switch(f_list[from-1][to-1])
      {
       case 1:
       *i32=i32ir(*i32);
       break;
       case 2:
       *i32=i32im(*i32);
       break;
       case 3:
       *i32=i32ri(*i32);
       break;
       case 4:
       *i32=i32rm(*i32);
       break;
       case 5:
       *i32=i32mi(*i32);
       break;
       case 6:
       *i32=i32mr(*i32);
       break;
      }
   }
fpreob32(f32,from,to)
  float *f32;
  int from,to;
   {
    switch(f_list[from-1][to-1])
      {
       case 1:
       *f32=f32ir(*f32);
       break;
       case 2:
       *f32=f32im(*f32);
       break;
       case 3:
       *f32=f32ri(*f32);
       break;
       case 4:
       *f32=f32rm(*f32);
       break;
       case 5:
       *f32=f32mi(*f32);
       break;
       case 6:
       *f32=f32mr(*f32);
       break;
      }
   }
fpreob64(f64,from,to)
  double *f64;
  int from,to;
   {
    switch(f_list[from-1][to-1])
      {
       case 1:
       *f64=f64ir(*f64);
       break;
       case 2:
       *f64=f64im(*f64);
       break;
       case 3:
       *f64=f64ri(*f64);
       break;
       case 4:
       *f64=f64rm(*f64);
       break;
       case 5:
       *f64=f64mi(*f64);
       break;
       case 6:
       *f64=f64mr(*f64);
       break;
      }
   }

