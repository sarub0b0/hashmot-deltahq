let SessionLoad = 1
let s:so_save = &so | let s:siso_save = &siso | set so=0 siso=0
let v:this_session=expand("<sfile>:p")
silent only
cd ~/work/hashmot/location_info_base/source
if expand('%') == '' && !&modified && line('$') <= 1 && getline(1) == ''
  let s:wipebuf = bufnr('%')
endif
set shortmess=aoO
badd +23 ~/work/hashmot/location_info_base/include/kdtree/kdtree.hh
badd +37 ~/work/hashmot/location_info_base/include/kdtree/kdtree_index.hh
badd +17 ~/work/hashmot/location_info_base/include/kdtree/tree.hh
badd +1 kdtree.cc
badd +23 kdtree/kdtree.cc
badd +27 lsh/lsh.cc
badd +0 kdtree/tree.cc
badd +28 ~/work/hashmot/location_info_base/test/axis_sort.cc
badd +0 kdtree/kdtree_index.cc
badd +27 ~/work/hashmot/location_info_base/include/lsh/l2hash.hh
badd +25 ~/work/hashmot/location_info_base/include/lsh/l2hash_family.hh
badd +13 main.cc
argglobal
silent! argdel *
edit ~/work/hashmot/location_info_base/include/kdtree/tree.hh
set splitbelow splitright
wincmd _ | wincmd |
vsplit
wincmd _ | wincmd |
vsplit
2wincmd h
wincmd _ | wincmd |
split
1wincmd k
wincmd w
wincmd w
wincmd _ | wincmd |
split
1wincmd k
wincmd w
wincmd w
wincmd _ | wincmd |
split
1wincmd k
wincmd w
set nosplitbelow
set nosplitright
wincmd t
set winminheight=1 winminwidth=1 winheight=1 winwidth=1
exe '1resize ' . ((&lines * 36 + 37) / 74)
exe 'vert 1resize ' . ((&columns * 84 + 127) / 254)
exe '2resize ' . ((&lines * 35 + 37) / 74)
exe 'vert 2resize ' . ((&columns * 84 + 127) / 254)
exe '3resize ' . ((&lines * 35 + 37) / 74)
exe 'vert 3resize ' . ((&columns * 84 + 127) / 254)
exe '4resize ' . ((&lines * 36 + 37) / 74)
exe 'vert 4resize ' . ((&columns * 84 + 127) / 254)
exe '5resize ' . ((&lines * 36 + 37) / 74)
exe 'vert 5resize ' . ((&columns * 84 + 127) / 254)
exe '6resize ' . ((&lines * 35 + 37) / 74)
exe 'vert 6resize ' . ((&columns * 84 + 127) / 254)
argglobal
setlocal fdm=manual
setlocal fde=0
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=0
setlocal fml=1
setlocal fdn=20
setlocal fen
silent! normal! zE
let s:l = 23 - ((22 * winheight(0) + 18) / 36)
if s:l < 1 | let s:l = 1 | endif
exe s:l
normal! zt
23
normal! 0
wincmd w
argglobal
if bufexists('kdtree/tree.cc') | buffer kdtree/tree.cc | else | edit kdtree/tree.cc | endif
setlocal fdm=manual
setlocal fde=0
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=0
setlocal fml=1
setlocal fdn=20
setlocal fen
silent! normal! zE
let s:l = 22 - ((20 * winheight(0) + 17) / 35)
if s:l < 1 | let s:l = 1 | endif
exe s:l
normal! zt
22
normal! 035|
wincmd w
argglobal
if bufexists('~/work/hashmot/location_info_base/include/kdtree/kdtree_index.hh') | buffer ~/work/hashmot/location_info_base/include/kdtree/kdtree_index.hh | else | edit ~/work/hashmot/location_info_base/include/kdtree/kdtree_index.hh | endif
setlocal fdm=manual
setlocal fde=0
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=0
setlocal fml=1
setlocal fdn=20
setlocal fen
silent! normal! zE
let s:l = 45 - ((27 * winheight(0) + 17) / 35)
if s:l < 1 | let s:l = 1 | endif
exe s:l
normal! zt
45
normal! 030|
wincmd w
argglobal
if bufexists('kdtree/kdtree_index.cc') | buffer kdtree/kdtree_index.cc | else | edit kdtree/kdtree_index.cc | endif
setlocal fdm=manual
setlocal fde=0
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=0
setlocal fml=1
setlocal fdn=20
setlocal fen
silent! normal! zE
let s:l = 113 - ((13 * winheight(0) + 18) / 36)
if s:l < 1 | let s:l = 1 | endif
exe s:l
normal! zt
113
normal! 012|
wincmd w
argglobal
if bufexists('~/work/hashmot/location_info_base/include/kdtree/kdtree.hh') | buffer ~/work/hashmot/location_info_base/include/kdtree/kdtree.hh | else | edit ~/work/hashmot/location_info_base/include/kdtree/kdtree.hh | endif
setlocal fdm=manual
setlocal fde=0
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=0
setlocal fml=1
setlocal fdn=20
setlocal fen
silent! normal! zE
let s:l = 24 - ((23 * winheight(0) + 18) / 36)
if s:l < 1 | let s:l = 1 | endif
exe s:l
normal! zt
24
normal! 0
wincmd w
argglobal
if bufexists('kdtree/kdtree.cc') | buffer kdtree/kdtree.cc | else | edit kdtree/kdtree.cc | endif
setlocal fdm=manual
setlocal fde=0
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=0
setlocal fml=1
setlocal fdn=20
setlocal fen
silent! normal! zE
let s:l = 28 - ((22 * winheight(0) + 17) / 35)
if s:l < 1 | let s:l = 1 | endif
exe s:l
normal! zt
28
normal! 013|
wincmd w
4wincmd w
exe '1resize ' . ((&lines * 36 + 37) / 74)
exe 'vert 1resize ' . ((&columns * 84 + 127) / 254)
exe '2resize ' . ((&lines * 35 + 37) / 74)
exe 'vert 2resize ' . ((&columns * 84 + 127) / 254)
exe '3resize ' . ((&lines * 35 + 37) / 74)
exe 'vert 3resize ' . ((&columns * 84 + 127) / 254)
exe '4resize ' . ((&lines * 36 + 37) / 74)
exe 'vert 4resize ' . ((&columns * 84 + 127) / 254)
exe '5resize ' . ((&lines * 36 + 37) / 74)
exe 'vert 5resize ' . ((&columns * 84 + 127) / 254)
exe '6resize ' . ((&lines * 35 + 37) / 74)
exe 'vert 6resize ' . ((&columns * 84 + 127) / 254)
tabnext 1
if exists('s:wipebuf') && getbufvar(s:wipebuf, '&buftype') isnot# 'terminal'
  silent exe 'bwipe ' . s:wipebuf
endif
unlet! s:wipebuf
set winheight=1 winwidth=20 winminheight=1 winminwidth=1 shortmess=filnxtToOF
let s:sx = expand("<sfile>:p:r")."x.vim"
if file_readable(s:sx)
  exe "source " . fnameescape(s:sx)
endif
let &so = s:so_save | let &siso = s:siso_save
doautoall SessionLoadPost
unlet SessionLoad
" vim: set ft=vim :
