(defun tdprof-hex-to-dword ()
  (interactive)
  (beginning-of-buffer)
  (query-replace-regexp "#\\(..\\),\\(..\\),\\(..\\),\\(..\\)" "#\\4\\3\\2\\1")
  (beginning-of-buffer)
  (query-replace-regexp "hex:\\(..\\),\\(..\\),\\(..\\),\\(..\\)" "dword:\\4\\3\\2\\1"))