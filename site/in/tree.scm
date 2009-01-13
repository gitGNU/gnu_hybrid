;;
;; tree.scm
;;
;; Copyright (C) 2007, 2008 Francesco Salvestrini
;;                          Alessandro Massignan
;;
;; This program is free software; you can redistribute it and/or modify
;; it under the terms of the GNU General Public License as published by
;; the Free Software Foundation; either version 2 of the License, or
;; (at your option) any later version.
;;
;; This program is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;; GNU General Public License for more details.
;;
;; You should have received a copy of the GNU General Public License along
;; with this program; if not, write to the Free Software Foundation, Inc.,
;; 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
;;

;; choosing a representation:
;; we could use built-in lists, for example,
;; '(3 (4 () ()) (5 () ())) can represent a tree with 3 at the root and
;; subtrees with data 4 and 5 on the left and right respectively, which
;; in turn have empty subtrees.  Another way to have trees (or more
;; complex structures in general, is to do it with lambda encapsulation
;; like we did above with "newcell", "head, and  "tail":

;; node takes a datum, and left and right branches to make a tree:
;; '() is again the empty tree (null)

(define tree-node
  (lambda (x l r)
    (lambda (s)
      (if (equal? s 'right) r
	  (if (equal? s 'left) l x)))))

;; convenient data accessors:
(define tree-data  (lambda (tree) (tree 'data)))
(define tree-left  (lambda (tree) (tree 'left)))
(define tree-right (lambda (tree) (tree 'right)))

;; computes number of nodes in the tree - try this
;; without recursion, even without tail-recursion!
(define tree-size
  (lambda (tree)
    (if (null? tree) 0
	(+ 1 (size (tree 'left)) (size (tree 'right))))))

;; length of longest branch of tree
(define depth
  (lambda (tree)
    (if (null? tree) 0
	(let ((ldepth (depth (left tree)))
	      (rdepth (depth (right tree))))
	  (+ 1 (if (> ldepth rdepth) ldepth rdepth))))))
;; note that "if" expressions return a value, which we can then add 1 to

;; determines if element x is present in the tree: - look ma, no if-else!
(define intree
  (lambda (x tree)
    (and (not (null? tree))
	 (or (equal? x (data tree))
	     (intree x (left tree))
	     (intree x (right tree))))))

;;(define mytree (node 6 (node 3 '() '()) (node 8 '() '())))

(define map-id
  (lambda (m)
    (car m)))

(define map-tree
  (lambda (m)
    (cdr m)))

(define node-id
  (lambda (n)
    (caar n)))

(define node-id-equal?
  (lambda (n i)
    (if (equal? (node-id n) i) #t #f)))

(define node-attributes
  (lambda (n)
    (car n)))

(define node-children
  (lambda (n)
    (cdr n)))

(define find-node
  (lambda (t i)
    (call-with-current-continuation
     (lambda (return)
       (map
	(lambda (n)
	  (if (not (null? n))
	      (if (node-id-equal? n i)
		  (return n)
		  (let ((r (find-node (node-children n) i)))
		    (if (not (null? r))
			(return r))))))
	t)
       '()))))
