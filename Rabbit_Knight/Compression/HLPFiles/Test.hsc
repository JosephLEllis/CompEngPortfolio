HelpScribble project file.
16
...
0
1
Compressor Help File



FALSE
0x0409   English (U.S.)
C:\Users\Jelli\OneDrive\Desktop\Rabbit_Knight\Compression,C:\Users\Jelli\OneDrive\Desktop\Rabbit_Knight\Compression\Images
3
BrowseButtons()
Back()
Contents()
0
FALSE
C:\Users\Jelli\OneDrive\Desktop\Rabbit_Knight\Compression\
FALSE
TRUE
16777215
0
16711680
8388736
255
FALSE
FALSE
FALSE
FALSE
150
50
600
500
TRUE
FALSE
1
TRUE
FALSE
Contents
%s Contents
Index
%s Index
Previous
Next
FALSE

7
10
Scribble10
Usage




Writing



FALSE
20
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil\fcharset0 Aptos;}{\f2\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red128\green128\blue128;\red0\green0\blue0;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\lang2057\b\fs32 How to use the RLE compression tool
\par \cf2\b0\fs24 ____________________________________________________________________________________________________________________________________________
\par 
\par \cf3\f1\fs22 The RLE compression tool provides allows you to compress data that has been output from the \cf4\strike GameBoy Map Builder \cf5\strike0\{linkID=14\} \cf3 or \cf4\strike GameBoy Tile Designer\cf5\strike0\{linkID=14\}\cf3 . 
\par 
\par You can toggle between standard Run Length Encoding (RLE) and RLE with control flags by checking/unchecking the checkbox at the bottom of the application window. 
\par 
\par In order to compress GBMB or GBTD output tilemaps or tile data, copy and paste the output into the upper textbox and click "Process". 
\par 
\par \cf5\{bmc process_bmp.bmp\}\cf3 
\par 
\par In order to clear the original uncompressed data from the upper application window textbox, click the "Clear" button. This will not erase the compressed output in the lower application window textbox.
\par 
\par \cf5\{bmc clear_bmp.bmp\}\cf2\f0\fs24 
\par \cf1\lang1033\b\f2\fs32 
\par 
\par 
\par }
11
Scribble11
Source code for standard RLE




Writing



FALSE
24
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil Courier New;}}
{\colortbl ;\red0\green0\blue255;\red128\green128\blue128;\red0\green0\blue0;\red255\green255\blue255;\red0\green128\blue0;\red128\green0\blue255;\red0\green0\blue128;\red255\green128\blue0;}
\viewkind4\uc1\pard\cf1\lang2057\b\fs32 Source code for standard RLE\cf0\lang1033\b0\f1\fs20 
\par \cf2\lang2057\f0\fs24 ____________________________________________________________________________________________________________________________________________
\par \cf3\highlight4\lang1033\f2\fs20 
\par 
\par 
\par \cf5 // Continuous RLE with max run length of 255
\par \cf6 int\cf3  rleEncode\cf7\b (\cf6\b0 const\cf3  \cf6 unsigned\cf3  \cf6 char\cf7\b *\cf3\b0  data\cf7\b ,\cf3\b0  \cf6 int\cf3  len\cf7\b ,\cf3\b0  \cf6 unsigned\cf3  \cf6 char\cf7\b *\cf3\b0  out\cf7\b ,\cf3\b0  \cf6 int\cf3  maxOut\cf7\b )\cf3\b0  \cf7\b\{\cf3\b0 
\par     \cf6 int\cf3  i \cf7\b =\cf3\b0  \cf8 0\cf7\b ,\cf3\b0  outPos \cf7\b =\cf3\b0  \cf8 0\cf7\b ;\cf3\b0 
\par     \cf1\b while\cf3\b0  \cf7\b (\cf3\b0 i \cf7\b <\cf3\b0  len \cf7\b &&\cf3\b0  outPos \cf7\b +\cf3\b0  \cf8 2\cf3  \cf7\b <=\cf3\b0  maxOut\cf7\b )\cf3\b0  \cf7\b\{\cf3\b0 
\par         \cf6 unsigned\cf3  \cf6 char\cf3  val \cf7\b =\cf3\b0  data\cf7\b [\cf3\b0 i\cf7\b ];\cf3\b0 
\par         \cf6 int\cf3  run \cf7\b =\cf3\b0  \cf8 1\cf7\b ;\cf3\b0 
\par         \cf1\b while\cf3\b0  \cf7\b (\cf3\b0 i \cf7\b +\cf3\b0  run \cf7\b <\cf3\b0  len \cf7\b &&\cf3\b0  data\cf7\b [\cf3\b0 i \cf7\b +\cf3\b0  run\cf7\b ]\cf3\b0  \cf7\b ==\cf3\b0  val \cf7\b &&\cf3\b0  run \cf7\b <\cf3\b0  \cf8 255\cf7\b )\cf3\b0  \cf7\b\{\cf3\b0 
\par             run\cf7\b ++;\cf3\b0 
\par         \cf7\b\}\cf3\b0 
\par         out\cf7\b [\cf3\b0 outPos\cf7\b ++]\cf3\b0  \cf7\b =\cf3\b0  \cf7\b (\cf6\b0 unsigned\cf3  \cf6 char\cf7\b )\cf3\b0 run\cf7\b ;\cf3\b0 
\par         out\cf7\b [\cf3\b0 outPos\cf7\b ++]\cf3\b0  \cf7\b =\cf3\b0  val\cf7\b ;\cf3\b0 
\par         i \cf7\b +=\cf3\b0  run\cf7\b ;\cf3\b0 
\par     \cf7\b\}\cf3\b0 
\par     \cf1\b return\cf3\b0  outPos\cf7\b ;\cf3\b0 
\par \cf7\b\}\cf3\b0 
\par \cf5 
\par }
12
Scribble12
Further reading




Writing



FALSE
7
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Aptos;}{\f3\fnil Courier New;}}
{\colortbl ;\red0\green0\blue255;\red128\green128\blue128;\red0\green0\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\lang2057\b\fs32 Further reading\cf0\lang1033\b0\f1\fs20 
\par \cf2\lang2057\f0\fs24 ____________________________________________________________________________________________________________________________________________
\par 
\par \cf3\f2\fs22 More information regarding RLE compression (and other compression methods) can be found at: \cf1\strike\f3\fs20 https://en.wikipedia.org/wiki/Run-length_encoding\cf4\strike0\{link=*! ExecFile("https://en.wikipedia.org/wiki/Run-length_encoding")\}\cf0\lang1033\f1 
\par }
13
Scribble13
Source code for RLE with control flags 




Writing



FALSE
46
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil Courier New;}{\f3\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;\red128\green128\blue128;\red0\green0\blue0;\red255\green255\blue255;\red0\green128\blue0;\red128\green0\blue255;\red0\green0\blue128;\red255\green128\blue0;}
\viewkind4\uc1\pard\cf1\lang2057\b\fs32 Source code for RLE with control flags \cf0\lang1033\b0\f1\fs20 
\par \cf2\lang2057\f0\fs24 ____________________________________________________________________________________________________________________________________________
\par \cf0\lang1033\f1\fs20 
\par \cf3\highlight4\f2 
\par 
\par \cf5 // \lang2057\f3 Modified RLE for use with control flags\cf3\lang1033\f2 
\par \cf6 void\cf3  rle_compress\cf7\b (\cf6\b0 const\cf3  \cf6 unsigned\cf3  \cf6 char\cf3  \cf7\b *\cf3\b0 input\cf7\b ,\cf3\b0  \cf6 int\cf3  input_len\cf7\b ,\cf3\b0  \cf6 unsigned\cf3  \cf6 char\cf3  \cf7\b *\cf3\b0 output\cf7\b ,\cf3\b0  \cf6 int\cf3  \cf7\b *\cf3\b0 out_len\cf7\b )\cf3\b0  \cf7\b\{\cf3\b0 
\par     \cf6 int\cf3  in \cf7\b =\cf3\b0  \cf8 0\cf7\b ,\cf3\b0  out \cf7\b =\cf3\b0  \cf8 0\cf7\b ;\cf3\b0 
\par 
\par     \cf1\b while\cf3\b0  \cf7\b (\cf3\b0 in \cf7\b <\cf3\b0  input_len\cf7\b )\cf3\b0  \cf7\b\{\cf3\b0 
\par         \cf6 int\cf3  run_len \cf7\b =\cf3\b0  \cf8 1\cf7\b ;\cf3\b0 
\par 
\par         \cf5 // Repeat run
\par \cf3         \cf1\b while\cf3\b0  \cf7\b (\cf3\b0 in \cf7\b +\cf3\b0  run_len \cf7\b <\cf3\b0  input_len \cf7\b &&\cf3\b0  input\cf7\b [\cf3\b0 in\cf7\b ]\cf3\b0  \cf7\b ==\cf3\b0  input\cf7\b [\cf3\b0 in \cf7\b +\cf3\b0  run_len\cf7\b ]\cf3\b0  \cf7\b &&\cf3\b0  run_len \cf7\b <\cf3\b0  \cf8 128\cf7\b )\cf3\b0  \cf7\b\{\cf3\b0 
\par             run_len\cf7\b ++;\cf3\b0 
\par         \cf7\b\}\cf3\b0 
\par 
\par         \cf1\b if\cf3\b0  \cf7\b (\cf3\b0 run_len \cf7\b >=\cf3\b0  \cf8 2\cf7\b )\cf3\b0  \cf7\b\{\cf3\b0 
\par             \cf5 // Encode repeat run
\par \cf3             output\cf7\b [\cf3\b0 out\cf7\b ++]\cf3\b0  \cf7\b =\cf3\b0  \cf8 0x80\cf3  \cf7\b |\cf3\b0  \cf7\b (\cf3\b0 run_len \cf7\b -\cf3\b0  \cf8 1\cf7\b );\cf3\b0 
\par             output\cf7\b [\cf3\b0 out\cf7\b ++]\cf3\b0  \cf7\b =\cf3\b0  input\cf7\b [\cf3\b0 in\cf7\b ];\cf3\b0 
\par             in \cf7\b +=\cf3\b0  run_len\cf7\b ;\cf3\b0 
\par         \cf7\b\}\cf3\b0  \cf1\b else\cf3\b0  \cf7\b\{\cf3\b0 
\par             \cf5 // Literal run
\par \cf3             \cf6 int\cf3  lit_start \cf7\b =\cf3\b0  in\cf7\b ;\cf3\b0 
\par             \cf6 int\cf3  lit_len \cf7\b =\cf3\b0  \cf8 1\cf7\b ;\cf3\b0 
\par             in\cf7\b ++;\cf3\b0 
\par 
\par             \cf1\b while\cf3\b0  \cf7\b (\cf3\b0 lit_len \cf7\b <\cf3\b0  \cf8 128\cf3  \cf7\b &&\cf3\b0  in \cf7\b <\cf3\b0  input_len\cf7\b )\cf3\b0  \cf7\b\{\cf3\b0 
\par                 \cf1\b if\cf3\b0  \cf7\b (\cf3\b0 in \cf7\b +\cf3\b0  \cf8 1\cf3  \cf7\b <\cf3\b0  input_len \cf7\b &&\cf3\b0  input\cf7\b [\cf3\b0 in\cf7\b ]\cf3\b0  \cf7\b ==\cf3\b0  input\cf7\b [\cf3\b0 in \cf7\b +\cf3\b0  \cf8 1\cf7\b ])\cf3\b0  \cf1\b break\cf7 ;\cf3\b0 
\par                 lit_len\cf7\b ++;\cf3\b0 
\par                 in\cf7\b ++;\cf3\b0 
\par             \cf7\b\}\cf3\b0 
\par 
\par             output\cf7\b [\cf3\b0 out\cf7\b ++]\cf3\b0  \cf7\b =\cf3\b0  lit_len \cf7\b -\cf3\b0  \cf8 1\cf7\b ;\cf3\b0 
\par             \cf1\b for\cf3\b0  \cf7\b (\cf6\b0 int\cf3  j \cf7\b =\cf3\b0  \cf8 0\cf7\b ;\cf3\b0  j \cf7\b <\cf3\b0  lit_len\cf7\b ;\cf3\b0  \cf7\b ++\cf3\b0 j\cf7\b )\cf3\b0  \cf7\b\{\cf3\b0 
\par                 output\cf7\b [\cf3\b0 out\cf7\b ++]\cf3\b0  \cf7\b =\cf3\b0  input\cf7\b [\cf3\b0 lit_start \cf7\b +\cf3\b0  j\cf7\b ];\cf3\b0 
\par             \cf7\b\}\cf3\b0 
\par         \cf7\b\}\cf3\b0 
\par     \cf7\b\}\cf3\b0 
\par 
\par     \cf7\b *\cf3\b0 out_len \cf7\b =\cf3\b0  out\cf7\b ;\cf3\b0 
\par \cf7\b\}\cf5\b0 
\par }
14
Scribble14
Required programs




Writing



FALSE
15
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Aptos;}{\f3\fnil Courier New;}}
{\colortbl ;\red0\green0\blue255;\red128\green128\blue128;\red0\green0\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\lang2057\b\fs32 Required programs\cf0\lang1033\b0\f1\fs20 
\par \cf2\lang2057\f0\fs24 ____________________________________________________________________________________________________________________________________________
\par \cf0\lang1033\f1\fs20 
\par \cf3\lang2057\f2\fs22 This program is designed to be used in conjunction with GameBoy Map Builder and GameBoy Tile Designer. 
\par 
\par \b GameBoy Map Builder can be downloaded from:\b0  
\par 
\par Alternatively, if the link is no longer available, it can be found under \cf1\strike\f3\fs20 https://www.devrs.com/gb/hmgd/gbmb.html\cf4\strike0\{link=*! ExecFile("https://www.devrs.com/gb/hmgd/gbmb.html")\}\cf3\f2\fs22 
\par 
\par \b GameBoy Tile Designer can be downloaded from: \b0 
\par 
\par Alternatively, if the link is no longer available, it can be found under\cf1\strike\f3\fs20  https://www.devrs.com/gb/hmgd/gbtd.html\cf4\strike0\{link=*!ExecFile("https://www.devrs.com/gb/hmgd/gbtd.html")\}
\par }
15
Scribble15
RLE compression methods




Writing



FALSE
33
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Aptos;}}
{\colortbl ;\red0\green0\blue255;\red128\green128\blue128;\red0\green0\blue0;}
\viewkind4\uc1\pard\cf1\lang2057\b\fs32 RLE compression\cf0\lang1033\b0\f1\fs20 
\par \cf2\lang2057\f0\fs24 ____________________________________________________________________________________________________________________________________________
\par 
\par \cf3\f2\fs22 The RLE compression tool uses two methods of compression; standard RLE and RLE with control flags. 
\par 
\par \b Standard RLE compression method: 
\par 
\par \b0 Run Length Encoding (RLE) is a compression method that stores sequences of repeated values as a pair in the following format: \i [run length] [value]\i0 .
\par 
\par Instead of storing every value separately, it encodes the compressed version as the number of times a certain value must be repeated. 
\par 
\par This generally works well for data that has long runs of identical values (e.g. tilemap backgrounds). 
\par 
\par \i The compression method used by this program has a maximum run length of 255. \b\i0 
\par 
\par RLE control flags method: 
\par 
\par \b0 This RLE variant encodes data using control bytes that distinguish between:\b 
\par \pard\tx200 
\par Repeat runs:  \b0 A control byte with the high bit set (0x80) followed by the repeated value.\b  
\par 
\par \b0 The lower 7 bits store (run length - 1).
\par \b 
\par \pard\b0 Maximum repeat run length = 128 bytes.\b 
\par 
\par Literal runs: \b0 A control byte with the high bit clear, followed by that many literal bytes.\b 
\par 
\par \b0 The function scans the input. If it finds a sequence of >= 2 identical bytes, then the value is stored as a repeat run.
\par 
\par Otherwise the algorithm will start a literal run until a repeat is encountered or max literal length is reached.\cf0\lang1033\f1\fs20 
\par }
20
Scribble20
Examples




Writing



FALSE
46
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Aptos;}}
{\colortbl ;\red0\green0\blue255;\red128\green128\blue128;\red0\green0\blue0;}
\viewkind4\uc1\pard\cf1\lang2057\b\fs32 Examples\cf0\lang1033\b0\f1\fs20 
\par \cf2\lang2057\f0\fs24 ____________________________________________________________________________________________________________________________________________
\par 
\par \cf3\b\f2\fs22 Standard RLE example: 
\par 
\par \b0 Consider the following \b 16-bytes\b0 :\b  DB\b0  $AA, $AA, $AA, $AA, $00, $00, $FF, $FF, $FF, $FF, $FF, $FF, $11, $11, $11, $22
\par 
\par     \b $AA\b0  repeats \b four times\b0 , which becomes: \b $04 $AA\b0 
\par     \b $00\b0  repeats \b two times\b0 , which becomes \b $02 $00\b0  
\par    \b  $FF \b0 repeats \b six times\b0 , which becomes \b $06 $FF\b0  
\par     \b $11\b0  repeats \b three times\b0 , which becomes \b $03 $11\b0  
\par     \b $22\b0  \b does not repeat\b0 , which becomes \b $01 $22\b0  
\par 
\par The sequence then becomes: \b DB\b0  $04, $AA, $02, $00, $06, $FF, $03, $11, $01, $22, which is now \b 10-bytes\b0 .  \b 
\par 
\par 
\par RLE with control flags example: \b0 
\par 
\par Consider the following \b 16-bytes\b0 :\b  DB\b0  $AA, $AA, $AA, $AA, $00, $00, $FF, $FF, $FF, $FF, $FF, $FF, $11, $11, $11, $22
\par 
\par     \b $AA\b0  repeats repeats four times, which becomes $80 \i OR\i0  (4 - 1) = $83, which becomes \b $83 $AA\b0 
\par     \b $00\b0  repeaats two times, which becomes $80 \i OR\i0  (2 - 1) = $81, which becomes \b $81 $00\b0 
\par     \b $FF\b0  repeats six times, which becomes $80 \i OR\i0  (6 - 1) = $85, which becomes \b $85 $FF\b0  
\par     \b $11\b0  repeats three times, which becomes $80 \i OR\i0  (3 - 1) = $82, which becomes\b  $82 $11\b0  
\par     \b $22\b0  does not repeat, which becomes $80 \i OR\i0  (1 - 1) = $00, which becomes \b $00 $22
\par \b0 
\par The sequence then becomes: \b DB\b0  $82, $AA, $81, $00, $85, $FF, $82, $11, $00, $22
\par 
\par The amount of compression differs depending on the data being compressed.
\par 
\par For example, consider the \b 16-byte\b0  sequence:\b  DB\b0  $01 $02 $03 $04 $05 $05 $05 $05 $05 $05 $06 $07 $08 $09 $0A $0B
\par 
\par \b Using standard RLE\b0 , the byte sequence becomes: \b DB\b0  $01, $01, $01, $02, $01, $03, $01, $04, $06, $05, $01, 
\par                                                                                     \b  DB\b0  $06, $01, $07, $01, $08, $01, $09, $01, $0A, $01, $0A, 
\par                                                                                      \b DB\b0  $01, $0B
\par 
\par \b Using standard RLE produces a "compressed" 22-byte sequence.\b0 
\par 
\par \b Using control flag RLE\b0 , the byte sequence becomes:\b  DB\b0  $03 $01 $02 $03 $04 $85 $05 $05 $06 $07 $08 $09 $0A $0B
\par 
\par \b Using control flag RLE produces a compressed 14-byte sequence. \b0 
\par 
\par As a general rule of thumb, use standard RLE if you have an absolute abundance of repeated values (e.g. a tilemap made of entirely black tiles) and control flag RLE for tilemaps with more varied and less repetitive patterns. \cf0\lang1033\f1\fs20 
\par }
0
0
0
14
1 Usage
2 Usage=Scribble10
1 Included algorithms
2 RLE compression methods=Scribble15
1 Examples
2 Examples=Scribble20
1 Required programs
2 Links to required programs=Scribble14
1 Further reading
2 Further reading=Scribble12
1 Source code for RLE with control flags 
2 C source code for RLE with control flags=Scribble13
1 Source code for standard RLE
2 C source code for standard RLE=Scribble11
6
*InternetLink
16711680
Courier New
0
10
1
....
0
0
0
0
0
0
*ParagraphTitle
-16777208
Arial
0
11
1
B...
0
0
0
0
0
0
*PopupLink
-16777208
Arial
0
8
1
....
0
0
0
0
0
0
*PopupTopicTitle
16711680
Arial
0
10
1
B...
0
0
0
0
0
0
*TopicText
-16777208
Arial
0
10
1
....
0
0
0
0
0
0
*TopicTitle
16711680
Arial
0
16
1
B...
0
0
0
0
0
0
