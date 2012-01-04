public int getCharWidth(char ch, char nextChar, char prevChar) {
    if(FIRST_TWO_CASES_CHAR <= ch && ch <= LASR_TWO_CASES_CHAR) {
        switch(ch) {
        /* LOOP_START */
            case ALEF:
                if(!isArabicChar(prev) || getCharCases(prev) == 2) {
                    return ALEF_CASE1_WIDTH;
                } else {
                    return ALEF_CASE2_WIDTH;
                }
                break;
        /* LOOP END */
            default:
                return 0;
        }
    } else {
        switch(ch) {
        /* LOOP_START */
            case MEM:
                if((!isArabicChar(prev) || getCharCases(prev) == 2) || (isArabicChar(nextChar) &&  getCharCases(prev) == 2)) {
                    return MEM_CASE1_WIDTH;
                } else if(isArabicChar(nextChar) &&  getCharCases(prev) == 4) {
                    return MEM_CASE2_WIDTH;
                } else if(!isArabicChar(nextChar) &&  getCharCases(prev) == 4) {
                    return MEM_CASE3_WIDTH;
                } else if(!isArabicChar(nextChar) &&  getCharCases(prev) == 2) {
                    return MEM_CASE4_WIDTH;
                } else {
                    System.err.println("What to do?");
                    return 0;
                }
                break;
        /* LOOP END */
            default:
                return 0;
        }
    }

}