#ifdef __cplusplus
extern "C" {
#endif
/****************************************************************************
data compression

lsize_threshold  - threshold size, >=0
nmethod  - compression method, =0 - automatic choice
buf_inp  - input data address
lbuf_inp - size of input data
buf_out  - output data address for any result
lbuf_out - size of output data for any result

result: =1 - no compression: buf_out[*]=buf_inp[*], lbuf_out=lbuf_inp
	=0 - compression
*/
extern
int Upack_compress( int lsize_threshold,
	            int nmethod,
	            void *buf_inp, int lbuf_inp,
	            void *buf_out, int *lbuf_out );
/****************************************************************************
data decompression for EC_CODE (no malloc)

buf_inp  - input data address
lbuf_inp - size of input data
buf_out  - output buffer  address
lbuf_out - size of output buffer

must be: lbuf_inp=lbuf_out

result: =1 - no decompression:
             buf_out[*]=buf_inp[*]
	     lbuf_out=lbuf_inp  - size of output data
	=0 - decompression
             lbuf_out - size of output data
	=2 - input data error
*/
extern
int Upack_decompressEC( void *buf_inp, int lbuf_inp,
	                void *buf_out, int *lbuf_out );
/****************************************************************************
data decompression

buf_inp  - input data address
lbuf_inp - size of input data
buf_out  - place for output data
lbuf_out - place for size of output data

must be: lbuf_inp=lbuf_out

result: =1 - no decompression
             buf_out  - output data address
             lbuf_out - size of output data
             buf_out[*]=buf_inp[*], lbuf_out=lbuf_inp
	=0 - decompression
             buf_out  - output data address
             lbuf_out - size of output data
	=2 - input data error
	=3 - malloc error
*/
extern
int Upack_decompress( void *buf_inp, int lbuf_inp,
	              void **buf_out, int *lbuf_out );
/**************************************************************************
*/
#ifdef __cplusplus
}
#endif
