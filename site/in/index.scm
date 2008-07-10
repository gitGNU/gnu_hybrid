(h3 "DESCRIPTION")
(p "Hybrid is a kernel built on top of "
   (a (@ (href "http://www.nongnu.org/elklib")) "elklib") "."
   "The kernel is being developed with an emphasis on design and portability. "
   "It is largely implemented in C/C++, with a small amount of assembly")

(h4 "Features")
(p "The kernel is under heavy development and is not yet usable from the user "
   "point of view. The available features are:"
   (ul
    (li "Strict architecture-independant layer that allows clean porting to other CPU architectures")
    (li "Debugging support with installable debugger commands")
    (li "Symbolic backtraces")
    (li "Multiboot compliance")
    (li "Memory partitions")
    (li "Kernel console")
    (li "Keyboard support")
    (li "IDT/GDT/BIOS/IRQs/TRAPs/DMA")
    )
   )

(h4 "Ports")
(p "The only available port is for Intel IA-32 (x86)"
   )

(h4 "Copying")
(p "Hybrid is licensed under the "
   (a (@ (href "http://www.gnu.org/licenses/licenses.html"))
      "GNU General Public License, version 2")
   )

(h3 "MAINTAINERS")
(p "Francesco Salvestrini <salvestrini AT gmail DOT com>"
 )

(h3 "AUTHORS")
(p "Francesco Salvestrini <salvestrini AT gmail DOT com>"
   )

(h3 "RELEASES")
(p "Sorry, no public release available at the moment.")

(h3 "MAILING LISTS")
;(p "Hybrid has several moderated mailing lists, each with an archive.
; For general Hybrid discussions, use <hybrid-user AT nongnu.org>.
; Email bug reports to <hybrid-bug AT nongnu.org>. For more information on
; submitting bugs, please see the section Report a Bug below.
; If you have a patch for a bug in Hybrid that hasn't yet been fixed in the
; latest repository sources, please send the patch (made for the git sources,
; not the release sources) to <hybrid-patch AT nongnu.org>."
; )
(p "Hybrid has several moderated mailing lists, each with an archive.
 For general Hybrid discussions, use <hybrid-user AT nongnu.org>.
 Email bug reports to <hybrid-bug AT nongnu.org>. For more information on
 submitting bugs, please see the section Report a Bug below.
 If you have a patch for a bug in Hybrid that hasn't yet been fixed in the
 latest repository sources, please send the patch (made for the git sources,
 not the release sources) to <hybrid-patch AT nongnu.org>."
 )

(h3 "REPORT A BUG")
(p "If you think you have found a bug in Hybrid, then please send as complete
 a report as possible to <hybrid-bug AT nongnu.org>. An easy way to collect all
 the required information, such as platform and compiler, is to run make check,
 and include the resulting file tests/testsuite.log to your report.
 Disagreements between the manual and the code are also bugs."
 )

(h3 "DEVELOPMENT")
(h4 "Browsing sources")
(p "You can Browse the "
   (a (@ (href "http://git.savannah.nongnu.org/gitweb/?p=hybrid.git"))
      "Git repository")
   " of this project with your web browser. This gives you a good
 picture of the current status of the source files. You may also view
 the complete histories of any file in the repository as well as
 differences among two versions."
   )
(h4 "Getting a Copy of the Git Repository")
(p "Anonymous checkout:")
(br)
(p (a (@ (href "http://savannah.gnu.org/maintenance/UsingGit")) git)
   " clone git://git.savannah.nongnu.org/hybrid.git")
