
;(add-to-list 'load-path "~/repo/XiongyiCui_WPI/emacs.init/")

; gnu global
(load-file "~/repo/XiongyiCui_WPI/emacs.init/gtags.el")
(setq c-mode-hook
          '(lambda ()
              (gtags-mode 1)
	      ))

;(require 'cl)
;(load-file "~/repo/XiongyiCui_WPI/emacs.init/package.el")

(load-theme 'tsdh-dark)

(setq user-full-name "Xiongyi Cui")
(setq user-mail-address "cuixiongyi2013@gmail.com")

; package archive

(package-initialize)
(add-to-list 'package-archives
             '("marmalade" . "http://marmalade-repo.org/packages/"))
(add-to-list 'package-archives
             '("melpa" . "http://melpa.milkbox.net/packages/") t)

(setq package-archive-enable-alist '(("melpa" deft magit)))



; marking text and clip
(delete-selection-mode t)
(transient-mark-mode t)
(setq x-select-enable-clipboard t)

; indention
(setq tab-width 2
      indent-tabs-mode nil)

; backup file
(setq make-backup-files nil)

;Yes and No





; (setq make-backup-files nil)
(defalias 'yes-or-no-p 'y-or-n-p)

					;
					; Key binding
(global-set-key (kbd "RET") 'newline-and-indent)
(global-set-key (kbd "C-;") 'comment-or-uncomment-region)
(global-set-key (kbd "M-/") 'hippie-expand)
(global-set-key (kbd "C-+") 'text-scale-increase)
(global-set-key (kbd "C--") 'text-scale-decrease)
(global-set-key (kbd "C-c C-k") 'compile)
(global-set-key (kbd "C-x g") 'magit-status)

