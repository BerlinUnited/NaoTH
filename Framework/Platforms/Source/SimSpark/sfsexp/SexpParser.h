/**
 * @file SexpParser.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu Yuan</a>
 * @breief encapsulate the s-expression parser,
 * about the sfsexp library please vist: http://sexpr.sourceforge.net/
 */

#ifndef SEXP_PARSER_H
#define SEXP_PARSER_H

#include <sfsexp/sexp.h>
#include <string>
#include <cstring>

class SexpParser
{
private:

    SexpParser()
    {
    }

public:

    /** parse the value of the sExp atom 
     *  @param [in] sexp sExp atom pointer
     *  @param [out] res the value
     *  @return if there is anything wrong FALSE, else TRUE
     */
    static bool parseValue(const sexp_t* sexp, std::string& res)
    {
        if (sexp && sexp->ty == SEXP_VALUE)
        {
            res = sexp->val;
            return true;
        }
        return false;
    }

    /** parse the value of the sExp atom 
     *  @param [in] sexp sExp atom pointer
     *  @param [out] res the value
     *  @return if there is anything wrong FALSE, else TRUE
     */
    static bool parseValue(const sexp_t* sexp, char &res)
    {
        if (sexp && sexp->ty == SEXP_VALUE)
        {
            res = sexp->val[0];
            return true;
        }
        return false;
    }

    /** parse the value of the sExp atom 
     *  @param [in] sexp sExp atom pointer
     *  @param [out] res the value
     *  @return if there is anything wrong FALSE, else TRUE
     */
    static bool parseValue(const sexp_t* sexp, int &res)
    {
        if (sexp && sexp->ty == SEXP_VALUE)
        {
            res = atoi(sexp->val);
            return true;
        }
        return false;
    }

    /** parse the value of the sExp atom 
     *  @param [in] sexp sExp atom pointer
     *  @param [out] res the value
     *  @return if there is anything wrong FALSE, else TRUE
     */
    static bool parseValue(const sexp_t* sexp, unsigned int &res)
    {
        if (sexp && sexp->ty == SEXP_VALUE)
        {
            res = static_cast<unsigned int> (atoi(sexp->val));
            return true;
        }
        return false;
    }

    /** parse the value of the sExp atom 
     *  @param [in] sexp Exp atom pointer
     *  @param [out] res the value
     *  @return if there is anything wrong FALSE, else TRUE
     */
    static bool parseValue(const sexp_t* sexp, float &res)
    {
        if (sexp && sexp->ty == SEXP_VALUE)
        {
            res = static_cast<float>(atof(sexp->val));
            return true;
        }
        return false;
    }

    /** parse the value of the sExp atom
     *  @param [in] sexp Exp atom pointer
     *  @param [out] res the value
     *  @return if there is anything wrong FALSE, else TRUE
     */
    static bool parseValue(const sexp_t* sexp, double &res)
    {
        if (sexp && sexp->ty == SEXP_VALUE)
        {
            res = atof(sexp->val);
            return true;
        }
        return false;
    }

    /** parse the value of the sExp atom 
     *  @param [in] sexp Exp atom pointer
     *  @param [out] res the value
     *  @return if there is anything wrong FALSE, else TRUE
     */
    static bool parseValue(const sexp_t* sexp, bool &res)
    {
        if (sexp && sexp->ty == SEXP_VALUE)
        {
            res = ('t' == *(sexp->val) || 'T' == *(sexp->val));
            return true;
        }
        return false;
    }

    /** parse the given value of the sExp list 
     *  @param [in] sexp sExp list pointer
     *  @param [in] given the name of given value
     *  @param [out] res the value
     *  @return if the value name is the same with given name,
     * and successfully get the value, return TRUE, otherwise return FALSE
     */
    template <class T>
    static bool parseGivenValue(const sexp_t* sexp, const char* given, T &res)
    {
        if (sexp && sexp->ty == SEXP_LIST)
        {
            const sexp_t* tmp = sexp->list;
            if (tmp->ty == SEXP_VALUE)
            {
                if (0 == strcmp(given, tmp->val))
                {
                    return parseValue(tmp->next, res);
                }
            }
            return false;
        }
        return false;
    }

    template <class T>
    static bool parseGivenValue(const sexp_t* sexp, const std::string& given, T &res)
    {
        return parseGivenValue<T > (sexp, given.c_str(), res);
    }

    /** parse a array value of the sExp atom 
     *  @param [in] sexp sExp atom pointer
     *  @param [in] size the size of array
     *  @param [out] res the value
     *  @return if there is anything wrong FALSE, else TRUE
     */
    template <class T>
    static bool parseArrayValue(const sexp_t* sexp, unsigned int size, T *res)
    {
        for (unsigned int i = 0; i < size; i++)
        {
            bool ok = parseValue(sexp, res[i]);
            if (!ok) return false;
            sexp = sexp->next;
        }
        return true;
    }

    template<typename T>
    static bool parseGivenArrayValue(const sexp_t* sexp, const char* given, unsigned int size, T *res)
    {
        if (sexp && sexp->ty == SEXP_LIST)
        {
            const sexp_t* tmp = sexp->list;
            if (tmp->ty == SEXP_VALUE)
            {
                if (0 == strcmp(given, tmp->val))
                {
                    return parseArrayValue(tmp->next, size, res);
                }
            }
            return false;
        }
        return false;
    }

    static bool isVal(const sexp_t* sexp)
    {
        return SEXP_VALUE == sexp->ty;
    }

    static bool isList(const sexp_t* sexp)
    {
        return SEXP_LIST == sexp->ty;
    }
};

#endif // SEXP_PARSER_H
