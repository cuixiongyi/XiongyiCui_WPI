
;(add-to-list 'load-path "~/repo/XiongyiCui_WPI/emacs.init/")

; gnu global
(load-file "~/repo/XiongyiCui_WPI/emacs.init/gtags.el")
(setq c++-mode-hook
          '(lambda ()
              (gtags-mode 1)
	      ))
(setq c-mode-hook
          '(lambda ()
              (gtags-mode 1)
	      ))

      (global-set-key (concat gtags-prefix-key "h") 'gtags-display-browser)
      (global-set-key "\C-]" 'gtags-find-tag-from-here)
      (global-set-key "\C-t" 'gtags-pop-stack)


(load-theme 'tsdh-dark)

(setq user-full-name "Xiongyi Cui")
(setq user-mail-address "cuixiongyi2013@gmail.com")

; package archive


;(require 'cl)
(load-file "~/repo/XiongyiCui_WPI/emacs.init/package.el")


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

(add-hook 'c++-mode-hook 'irony-mode)
(add-hook 'c-mode-hook 'irony-mode)
(add-hook 'objc-mode-hook 'irony-mode)

;; replace the `completion-at-point' and `complete-symbol' bindings in
;; irony-mode's buffers by irony-mode's function
(defun my-irony-mode-hook ()
  (define-key irony-mode-map [remap completion-at-point]
    'irony-completion-at-point-async)
  (define-key irony-mode-map [remap complete-symbol]
    'irony-completion-at-point-async))
(add-hook 'irony-mode-hook 'my-irony-mode-hook)
(add-hook 'irony-mode-hook 'irony-cdb-autosetup-compile-options)

; switch between buffer
; C-x b

; go back to where the last mark is 
; C-x C-SPC



