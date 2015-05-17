

(defvar xiongyi/packages '(ac-slime
                          auto-complete
                          autopair
                          clojure-mode
                          clojure-test-mode
                          coffee-mode
                          csharp-mode
                          deft
                          erlang
                          feature-mode
                          flycheck
                          gist
                          go-mode
                          graphviz-dot-mode
                          haml-mode
                          ;haskell-mode
                          htmlize
                          magit
                          markdown-mode
                          marmalade
                          ;nodejs-repl
                          nrepl
                          o-blog
                          org
                          paredit
                          php-mode
                          puppet-mode
                          restclient
                          rvm
                          scala-mode
                          smex
                          sml-mode
                          ;solarized-theme
                          web-mode
                          writegood-mode
                          yaml-mode)
  "Default packages")


(defun xiongyi/packages-installed-p ()
  (loop for pkg in xiongyi/packages
        when (not (package-installed-p pkg)) do (return nil)
        finally (return t)))

(unless (xiongyi/packages-installed-p)
  (message "%s" "Refreshing package database...")
  (package-refresh-contents)
  (dolist (pkg xiongyi/packages)
    (when (not (package-installed-p pkg))
      (package-install pkg))))
