#include "editdistance.h"

/* constructor
 *
 * it populates all the Broad Sound Groups (BSGs)
 * Consonants are grouped based on CMU phonetic dictionaries breakdown.
 * Vowels are brokendown by myself, playing it by the ear.
 * See the .h file for more details
*/
EditDistance::EditDistance()
{
    //    monophthongs.append("AO");
    //    monophthongs.append("AO0");
    //    monophthongs.append("AO1");
    //    monophthongs.append("AO2");

        customGroup1.append("AO");
        customGroup1.append("AO0");
        customGroup1.append("AO1");
        customGroup1.append("AO2");

    //    monophthongs.append("AA");
    //    monophthongs.append("AA0");
    //    monophthongs.append("AA1");
    //    monophthongs.append("AA2");

        customGroup2.append("AA");
        customGroup2.append("AA0");
        customGroup2.append("AA1");
        customGroup2.append("AA2");

    //    monophthongs.append("IY");
    //    monophthongs.append("IY0");
    //    monophthongs.append("IY1");
    //    monophthongs.append("IY2");

        customGroup3.append("IY");
        customGroup3.append("IY0");
        customGroup3.append("IY1");
        customGroup3.append("IY2");

    //    monophthongs.append("UW");
    //    monophthongs.append("UW0");
    //    monophthongs.append("UW1");
    //    monophthongs.append("UW2");

        customGroup1.append("UW");
        customGroup1.append("UW0");
        customGroup1.append("UW1");
        customGroup1.append("UW2");

    //    monophthongs.append("EH");
    //    monophthongs.append("EH0");
    //    monophthongs.append("EH1");
    //    monophthongs.append("EH2");

        customGroup2.append("EH");
        customGroup2.append("EH0");
        customGroup2.append("EH1");
        customGroup2.append("EH2");

    //    monophthongs.append("IH");
    //    monophthongs.append("IH0");
    //    monophthongs.append("IH1");
    //    monophthongs.append("IH2");

        customGroup3.append("IH");
        customGroup3.append("IH0");
        customGroup3.append("IH1");
        customGroup3.append("IH2");

    //    monophthongs.append("UH");
    //    monophthongs.append("UH0");
    //    monophthongs.append("UH1");
    //    monophthongs.append("UH2");

        customGroup1.append("UH");
        customGroup1.append("UH0");
        customGroup1.append("UH1");
        customGroup1.append("UH2");

    //    monophthongs.append("AH");
    //    monophthongs.append("AH0");
    //    monophthongs.append("AH1");
    //    monophthongs.append("AH2");

        customGroup2.append("AH");
        customGroup2.append("AH0");
        customGroup2.append("AH1");
        customGroup2.append("AH2");

    //    monophthongs.append("AX");
    //    monophthongs.append("AX0");
    //    monophthongs.append("AX1");
    //    monophthongs.append("AX2");

        customGroup3.append("AX");
        customGroup3.append("AX0");
        customGroup3.append("AX1");
        customGroup3.append("AX2");

    //    monophthongs.append("AE");
    //    monophthongs.append("AE0");
    //    monophthongs.append("AE1");
    //    monophthongs.append("AE2");

        customGroup2.append("AE");
        customGroup2.append("AE0");
        customGroup2.append("AE1");
        customGroup2.append("AE2");

    //    diphthongs.append("EY");
    //    diphthongs.append("EY0");
    //    diphthongs.append("EY1");
    //    diphthongs.append("EY2");

        customGroup4.append("EY");
        customGroup4.append("EY0");
        customGroup4.append("EY1");
        customGroup4.append("EY2");

    //    diphthongs.append("AY");
    //    diphthongs.append("AY0");
    //    diphthongs.append("AY1");
    //    diphthongs.append("AY2");

        customGroup4.append("AY");
        customGroup4.append("AY0");
        customGroup4.append("AY1");
        customGroup4.append("AY2");

    //    diphthongs.append("OW");
    //    diphthongs.append("OW0");
    //    diphthongs.append("OW1");
    //    diphthongs.append("OW2");

        customGroup1.append("OW");
        customGroup1.append("OW0");
        customGroup1.append("OW1");
        customGroup1.append("OW2");

    //    diphthongs.append("AW");
    //    diphthongs.append("AW0");
    //    diphthongs.append("AW1");
    //    diphthongs.append("AW2");

        customGroup1.append("AW");
        customGroup1.append("AW0");
        customGroup1.append("AW1");
        customGroup1.append("AW2");

    //    diphthongs.append("OY");
    //    diphthongs.append("OY0");
    //    diphthongs.append("OY1");
    //    diphthongs.append("OY2");

        customGroup4.append("OY");
        customGroup4.append("OY0");
        customGroup4.append("OY1");
        customGroup4.append("OY2");

        rColoureds.append("ER");
        rColoureds.append("ER0");
        rColoureds.append("ER1");
        rColoureds.append("ER2");

        semivowels.append("W");
        semivowels.append("Y");

        stops.append("B");
        stops.append("D");
        stops.append("G");
        stops.append("K");
        stops.append("P");
        stops.append("T");

        affricates.append("CH");
        affricates.append("JH");

        fricatives.append("DH");
        fricatives.append("F");
        fricatives.append("S");
        fricatives.append("SH");
        fricatives.append("TH");
        fricatives.append("V");
        fricatives.append("Z");
        fricatives.append("ZH");

        aspirates.append("HH");

        liquids.append("L");
        liquids.append("R");

        nasals.append("M");
        nasals.append("N");
        nasals.append("NG");

    //    phonemes.append(monophthongs);
    //    phonemes.append(diphthongs);
        phonemes.append(customGroup1);
        phonemes.append(customGroup2);
        phonemes.append(customGroup3);
        phonemes.append(customGroup4);
        phonemes.append(rColoureds);

        phonemes.append(semivowels);
        phonemes.append(stops);
        phonemes.append(affricates);
        phonemes.append(fricatives);
        phonemes.append(aspirates);
        phonemes.append(liquids);
        phonemes.append(nasals);
}

// a wrapper for Dean's code. Dean's methods are commented in blue.
double EditDistance::getDistance(QStringList input1, QStringList input2)
{
    return getEditDistance(input1, input2);
}


/**
 * Calculates the best case for edit distance on the phonemes
 * between two words. It works just like the regular dynamic
 * programming edit distance algorithm but with phonemes instead
 * of characters. Phonemes may also have varying replacement
 * values (see getReplacement)
 *
 * @param word1 array of phonemes
 * @param word2 array of phonemes
 * @return edit distance on phonemes between two words
 */
double EditDistance::getEditDistance(QStringList word1, QStringList word2)
{
    int len1 = word1.size();
    int len2 = word2.size();

    //creating the dynamic programming table
    // len1+1, len2+1, because finally return dp[len1][len2]
    double dp[len1 + 1][len2 + 1];

    //initializing the table
    for (int i = 0; i <= len1; i++) {
        dp[i][0] = i;
    }

    for (int j = 0; j <= len2; j++) {
        dp[0][j] = j;
    }

    //iterate though, and check last char
    for (int i = 0; i < len1; i++) {
        QString p1 = word1.at(i);
        for (int j = 0; j < len2; j++) {
            QString p2 = word2.at(j);
            if (p1.compare(p2) == 0) { // If the phonemes are the same
                //update dp value for +1 length
                dp[i + 1][j + 1] = dp[i][j];
            } else {
                double replace = dp[i][j] + getReplace(p1,p2);
                double insert = dp[i][j + 1] + 1;
                double omit = dp[i + 1][j] + 1;

                // if replace > insert, min = insert, else min = replace
                double min = replace > insert ? insert : replace;
                // if delete > min, min = min, else min = delete
                min = omit > min ? min : omit;
                dp[i + 1][j + 1] = min;
            }
        }
    }
    return dp[len1][len2];
}

/**
 * Calculates the value of replacement.
 * 0.5 if they are in the same sound group
 * 1 if they are not in the same sound group
 *
 * NOTE: Should not compare two phonemes that are the same.
 *
 * @param phoneme1
 * @param phoneme2
 * @return replacement Value
 */
double EditDistance::getReplace(QString p1, QString p2)
{
    bool trigger1 = false;
    bool trigger2 = false;

    // for each list
    for(int i = 0; i < phonemes.size(); i++){
        // for each phoneme in the list
        for(int j = 0; j < phonemes.at(i).size(); j++){
            QString current = phonemes[i][j];
            // if current phoneme is p1 or p2
            if(current == p1){
                trigger1 = true;
            }else if(current == p2){
                trigger2 = true;
            }
        }
        // if both phonemes are in the current list
        if(trigger1 == true && trigger2 == true){
            return 0.5;
            //if only one phoneme is in the current list
        }else if(trigger1 == true && trigger2 == false){
            return 1;
        }else if(trigger1 == false && trigger2 == true){
            return 1;
        }
    }
    // if both the phonemes are not in any list
    return 1;
}

EditDistance::~EditDistance()
{

}

