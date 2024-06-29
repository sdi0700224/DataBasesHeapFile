ΦΟΙΤΗΤΕΣ ΠΟΥ ΥΛΟΠΟΙΗΣΑΝ ΤΗΝ ΕΡΓΑΣΙΑ
-ΡΟΥΣΣΟΣ ΒΑΣΙΛΕΙΟΣ 1115200700224
-ΔΗΜΑΚΑΚΟΣ ΠΑΝΑΓΙΩΤΗΣ 1115202000268
-ΟΡΦΑΝΑΚΟΥ ΦΟΙΒΗ 1115202000156

ΠΑΡΑΤΗΡΗΣΕΙΣ

-Για τις δομή HP_info θεωρούμε οτι το id του τελευταίου block είναι το ίδιο με το count των block
-Για τις δομή HP_Block_info δεν βάλαμε δείκτη όπως έλεγε η εκφώνηση, διότι αφού πρόκειται για block που επιστέφονται σε δίσκο
 η διεύθυνση μπορεί πλέον να μη περιέχει το block που θέλουμε
-Αρκετά σχόλια υπάρχουν και στον κώδικά μας

Για την δημιουργία του εκτελέσιμου το οποίο δείχνει
τις δυνατότητες της βιβλιοθήκης BF, τρέξτε την εντολή:

make hp;

Για να τρέξετε το εκτελέσιμο:

./build/bf_main

# Known Issues

-For big inputs libbf.so library that is being used for the communication with the OS may throw an error. This bug is not part
 of this implementation but it's effect is being seen here.