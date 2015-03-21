set runtimepath+=~/.vim_runtime

source ~/.vim_runtime/vimrcs/basic.vim
source ~/.vim_runtime/vimrcs/filetypes.vim
source ~/.vim_runtime/vimrcs/plugins_config.vim
source ~/.vim_runtime/vimrcs/extended.vim

try
source ~/.vim_runtime/my_configs.vim
catch
endtry


set nocompatible              " be iMproved, required
filetype off                  " required

" set the runtime path to include Vundle and initialize
set rtp+=~/.vim/bundle/Vundle.vim
call vundle#begin()
" alternatively, pass a path where Vundle should install plugins
"call vundle#begin('~/some/path/here')

" let Vundle manage Vundle, required
Plugin 'gmarik/Vundle.vim'

" The following are examples of different formats supported.
" Keep Plugin commands between vundle#begin/end.
" plugin on GitHub repo
Plugin 'https://github.com/scrooloose/nerdtree.git'

Plugin 'https://github.com/scrooloose/syntastic.git'
"Plugin 'https://github.com/Lokaltog/vim-easymotion.git'
"Plugin 'https://github.com/tpope/vim-fugitive.git'
Plugin 'https://github.com/Valloric/YouCompleteMe.git'


" All of your Plugins must be added before the following line
call vundle#end()            " required
filetype plugin indent on    " required
" To ignore plugin indent changes, instead use:
"filetype plugin on
"
" Brief help
" :PluginList       - lists configured plugins
" :PluginInstall    - installs plugins; append `!` to update or just :PluginUpdate
" :PluginSearch foo - searches for foo; append `!` to refresh local cache
" :PluginClean      - confirms removal of unused plugins; append `!` to auto-approve removal
"
" see :h vundle for more details or wiki for FAQ
" Put your non-Plugin stuff after this line
let mapleader = "9"
let g:mapleader = "9"
nmap <leader>w :w!<cr>

"gtags
:map <C-]> :Gtags<CR><CR>
:map <C-[> :Gtags -r<CR><CR>
:map <F10> :cclose<CR>
:map <F11> :cp<CR>
vmap r "_dP
:map <F12> :cn<CR>

"NERDTree
map <Leader>0 :NERDTreeToggle<CR>

" tab navigation like firefox
nnoremap <C-S-tab> :tabprevious<CR>
nnoremap <C-tab>   :tabnext<CR>
nnoremap <C-t>     :tabnew<CR>
inoremap <C-S-tab> <Esc>:tabprevious<CR>i
inoremap <C-tab>   <Esc>:tabnext<CR>i
inoremap <C-t>     <Esc>:tabnew<CR>
inoremap <C-w>     <Esc>:tabclose<CR>

"Zoom in Ctrl Shift +.
"Zoom out with Ctrl -

"Open h/cpp file using fswitch
nmap <Leader>h :FSRight<cr>
"Switch to the file and load it into a new window split on the right >
nmap <Leader>hs :FSSplitRight<cr>

"use 8 to search instead *
map 8 *<cr>
"r is to delete 1 word and paste
nnoremap r "_dwP
vnoremap r "_dwP
nnoremap R "_dP
vnoremap R "_dP
" delete without copy into clipboard
nnoremap d "_d
vnoremap d "_d
nnoremap D "_D
vnoremap D "_D
nnoremap c "_c
vnoremap c "_c
nnoremap C "_C
vnoremap C "_C

"CXY_Setting_End
"==========================================================================
