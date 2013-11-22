/*
 * Moka : Un modeleur de 3-G-cartes.
 * Copyright (C) 2004, Moka Team, Université de Poitiers, Laboratoire SIC
 *               http://www.sic.sp2mi.univ-poitiers.fr/
 * Copyright (C) 2009, Guillaume Damiand, CNRS, LIRIS,
 *               guillaume.damiand@liris.cnrs.fr, http://liris.cnrs.fr/
 *
 * This file is part of Moka
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtCore/QtAlgorithms>
#include <QtCore/QRegExp>
#include <QtCore/QStringList>
#include <QtCore/QtDebug>

#include "HtmlEntities.h"



const int HTML::MaxEntities = 248;
const int HTML::FirstEntities = 10;


const HtmlEntity HTML::first[FirstEntities] =
{
    { "acirc", 0x00e2 },
    { "agrave", 0x00e0 },
    { "ccedil", 0x00e7 },
    { "eacute", 0x00e9 },
    { "ecirc", 0x00ea },
    { "egrave", 0x00e8 },
    { "icirc", 0x00ee },
    { "ocirc", 0x00f4 },
    { "ucirc", 0x00fb },
    { "ugrave", 0x00f9 },
};

const HtmlEntity HTML::entities[MaxEntities] =
{
    { "AElig", 0x00c6 },
    { "Aacute", 0x00c1 },
    { "Acirc", 0x00c2 },
    { "Agrave", 0x00c0 },
    { "Alpha", 0x0391 },
    { "AMP", 38 },
    { "Aring", 0x00c5 },
    { "Atilde", 0x00c3 },
    { "Auml", 0x00c4 },
    { "Beta", 0x0392 },
    { "Ccedil", 0x00c7 },
    { "Chi", 0x03a7 },
    { "Dagger", 0x2021 },
    { "Delta", 0x0394 },
    { "ETH", 0x00d0 },
    { "Eacute", 0x00c9 },
    { "Ecirc", 0x00ca },
    { "Egrave", 0x00c8 },
    { "Epsilon", 0x0395 },
    { "Eta", 0x0397 },
    { "Euml", 0x00cb },
    { "Gamma", 0x0393 },
    { "GT", 62 },
    { "Iacute", 0x00cd },
    { "Icirc", 0x00ce },
    { "Igrave", 0x00cc },
    { "Iota", 0x0399 },
    { "Iuml", 0x00cf },
    { "Kappa", 0x039a },
    { "Lambda", 0x039b },
    { "LT", 60 },
    { "Mu", 0x039c },
    { "Ntilde", 0x00d1 },
    { "Nu", 0x039d },
    { "OElig", 0x0152 },
    { "Oacute", 0x00d3 },
    { "Ocirc", 0x00d4 },
    { "Ograve", 0x00d2 },
    { "Omega", 0x03a9 },
    { "Omicron", 0x039f },
    { "Oslash", 0x00d8 },
    { "Otilde", 0x00d5 },
    { "Ouml", 0x00d6 },
    { "Phi", 0x03a6 },
    { "Pi", 0x03a0 },
    { "Prime", 0x2033 },
    { "Psi", 0x03a8 },
    { "QUOT", 34 },
    { "Rho", 0x03a1 },
    { "Scaron", 0x0160 },
    { "Sigma", 0x03a3 },
    { "THORN", 0x00de },
    { "Tau", 0x03a4 },
    { "Theta", 0x0398 },
    { "Uacute", 0x00da },
    { "Ucirc", 0x00db },
    { "Ugrave", 0x00d9 },
    { "Upsilon", 0x03a5 },
    { "Uuml", 0x00dc },
    { "Xi", 0x039e },
    { "Yacute", 0x00dd },
    { "Yuml", 0x0178 },
    { "Zeta", 0x0396 },
    { "aacute", 0x00e1 },
    { "acute", 0x00b4 },
    { "aelig", 0x00e6 },
    { "alefsym", 0x2135 },
    { "alpha", 0x03b1 },
    { "amp", 38 },
    { "and", 0x22a5 },
    { "ang", 0x2220 },
    { "apos", 0x0027 },
    { "aring", 0x00e5 },
    { "asymp", 0x2248 },
    { "atilde", 0x00e3 },
    { "auml", 0x00e4 },
    { "bdquo", 0x201e },
    { "beta", 0x03b2 },
    { "brvbar", 0x00a6 },
    { "bull", 0x2022 },
    { "cap", 0x2229 },
    { "cedil", 0x00b8 },
    { "cent", 0x00a2 },
    { "chi", 0x03c7 },
    { "circ", 0x02c6 },
    { "clubs", 0x2663 },
    { "cong", 0x2245 },
    { "copy", 0x00a9 },
    { "crarr", 0x21b5 },
    { "cup", 0x222a },
    { "cur" "ren", 0x00a4 },
    { "dArr", 0x21d3 },
    { "dagger", 0x2020 },
    { "darr", 0x2193 },
    { "deg", 0x00b0 },
    { "delta", 0x03b4 },
    { "diams", 0x2666 },
    { "divide", 0x00f7 },
    { "empty", 0x2205 },
    { "emsp", 0x2003 },
    { "ensp", 0x2002 },
    { "epsilon", 0x03b5 },
    { "equiv", 0x2261 },
    { "eta", 0x03b7 },
    { "eth", 0x00f0 },
    { "euml", 0x00eb },
    { "euro", 0x20ac },
    { "exist", 0x2203 },
    { "fnof", 0x0192 },
    { "forall", 0x2200 },
    { "frac12", 0x00bd },
    { "frac14", 0x00bc },
    { "frac34", 0x00be },
    { "frasl", 0x2044 },
    { "gamma", 0x03b3 },
    { "ge", 0x2265 },
    { "gt", 62 },
    { "hArr", 0x21d4 },
    { "harr", 0x2194 },
    { "hearts", 0x2665 },
    { "hellip", 0x2026 },
    { "iacute", 0x00ed },
    { "iexcl", 0x00a1 },
    { "igrave", 0x00ec },
    { "image", 0x2111 },
    { "infin", 0x221e },
    { "int", 0x222b },
    { "iota", 0x03b9 },
    { "iquest", 0x00bf },
    { "isin", 0x2208 },
    { "iuml", 0x00ef },
    { "kappa", 0x03ba },
    { "lArr", 0x21d0 },
    { "lambda", 0x03bb },
    { "lang", 0x2329 },
    { "laquo", 0x00ab },
    { "larr", 0x2190 },
    { "lceil", 0x2308 },
    { "ldquo", 0x201c },
    { "le", 0x2264 },
    { "lfloor", 0x230a },
    { "lowast", 0x2217 },
    { "loz", 0x25ca },
    { "lrm", 0x200e },
    { "lsaquo", 0x2039 },
    { "lsquo", 0x2018 },
    { "lt", 60 },
    { "macr", 0x00af },
    { "mdash", 0x2014 },
    { "micro", 0x00b5 },
    { "middot", 0x00b7 },
    { "minus", 0x2212 },
    { "mu", 0x03bc },
    { "nabla", 0x2207 },
    { "nbsp", 0x00a0 },
    { "ndash", 0x2013 },
    { "ne", 0x2260 },
    { "ni", 0x220b },
    { "not", 0x00ac },
    { "notin", 0x2209 },
    { "nsub", 0x2284 },
    { "ntilde", 0x00f1 },
    { "nu", 0x03bd },
    { "oacute", 0x00f3 },
    { "oelig", 0x0153 },
    { "ograve", 0x00f2 },
    { "oline", 0x203e },
    { "omega", 0x03c9 },
    { "omicron", 0x03bf },
    { "oplus", 0x2295 },
    { "or", 0x22a6 },
    { "ordf", 0x00aa },
    { "ordm", 0x00ba },
    { "oslash", 0x00f8 },
    { "otilde", 0x00f5 },
    { "otimes", 0x2297 },
    { "ouml", 0x00f6 },
    { "para", 0x00b6 },
    { "part", 0x2202 },
    { "percnt", 0x0025 },
    { "permil", 0x2030 },
    { "perp", 0x22a5 },
    { "phi", 0x03c6 },
    { "pi", 0x03c0 },
    { "piv", 0x03d6 },
    { "plusmn", 0x00b1 },
    { "pound", 0x00a3 },
    { "prime", 0x2032 },
    { "prod", 0x220f },
    { "prop", 0x221d },
    { "psi", 0x03c8 },
    { "quot", 34 },
    { "rArr", 0x21d2 },
    { "radic", 0x221a },
    { "rang", 0x232a },
    { "raquo", 0x00bb },
    { "rarr", 0x2192 },
    { "rceil", 0x2309 },
    { "rdquo", 0x201d },
    { "real", 0x211c },
    { "reg", 0x00ae },
    { "rfloor", 0x230b },
    { "rho", 0x03c1 },
    { "rlm", 0x200f },
    { "rsaquo", 0x203a },
    { "rsquo", 0x2019 },
    { "sbquo", 0x201a },
    { "scaron", 0x0161 },
    { "sdot", 0x22c5 },
    { "sect", 0x00a7 },
    { "shy", 0x00ad },
    { "sigma", 0x03c3 },
    { "sigmaf", 0x03c2 },
    { "sim", 0x223c },
    { "spades", 0x2660 },
    { "sub", 0x2282 },
    { "sube", 0x2286 },
    { "sum", 0x2211 },
    { "sup1", 0x00b9 },
    { "sup2", 0x00b2 },
    { "sup3", 0x00b3 },
    { "sup", 0x2283 },
    { "supe", 0x2287 },
    { "szlig", 0x00df },
    { "tau", 0x03c4 },
    { "there4", 0x2234 },
    { "theta", 0x03b8 },
    { "thetasym", 0x03d1 },
    { "thinsp", 0x2009 },
    { "thorn", 0x00fe },
    { "tilde", 0x02dc },
    { "times", 0x00d7 },
    { "trade", 0x2122 },
    { "uArr", 0x21d1 },
    { "uacute", 0x00fa },
    { "uarr", 0x2191 },
    { "uml", 0x00a8 },
    { "upsih", 0x03d2 },
    { "upsilon", 0x03c5 },
    { "uuml", 0x00fc },
    { "weierp", 0x2118 },
    { "xi", 0x03be },
    { "yacute", 0x00fd },
    { "yen", 0x00a5 },
    { "yuml", 0x00ff },
    { "zeta", 0x03b6 },
    { "zwj", 0x200d },
    { "zwnj", 0x200c }
};


const HtmlEntity* HTML::end_ent = &entities[MaxEntities];
const HtmlEntity* HTML::start_ent = &entities[0];

const HtmlEntity* HTML::end_first = &first[FirstEntities];
const HtmlEntity* HTML::start_first = &first[0];


static bool operator < (const QString &entityStr, const HtmlEntity &entity){
    return entityStr < QLatin1String(entity.name);
}


static bool operator < (const HtmlEntity &entity, const QString &entityStr){
    return QLatin1String(entity.name) < entityStr;
}


static bool operator == (const HtmlEntity &entity, const quint16 entityCode){
    return entityCode == entity.code;
}

// Retourne le caractere associe a un caractere special HTML.

QChar HTML::resolveEntity(const QString &entity)
{
    HtmlEntity* he = (HtmlEntity*) qBinaryFind(start_first,end_first,entity);
    if (he != end_first) return he->code;
    he = (HtmlEntity*) qBinaryFind(start_ent,end_ent,entity);
    if (he == end_ent) return QChar();
    return he->code;
}




// Retourne la chaine HTML representant le caractere "entityCode".

QString HTML::resolveEntity(quint16 entityCode)
{
    HtmlEntity* he = (HtmlEntity*) qFind(start_first,end_first,entityCode);
    if (he != end_first) return he->name;
    he = (HtmlEntity*) qFind(start_ent,end_ent,entityCode);
    if (he == end_ent) return QString();
    return he->name;
}



// ------------------------------------------------------------------
//                      Fonction de decodage
// ------------------------------------------------------------------



// Decode une chaine de caractere utilisant des
// caracteres speciaux d'HTML.

void HTML::decodeStr(QString* str)
{
    int index = 0;
    QChar decodedChar;
    QStringList res;
    QRegExp entityParser("(&([a-zA-Z]+);)",Qt::CaseInsensitive);
    QRegExp directCharParser("(&#([0-9]+);)",Qt::CaseInsensitive);


    qDebug() << "Checking" << *str;
    while (index > -1) {
        index = directCharParser.indexIn(*str,index+1);
        res = directCharParser.capturedTexts();
        if (index != -1) {
            decodedChar = QChar(directCharParser.cap(2).toInt());
            str->replace(res.at(1), decodedChar);
            qDebug() << *str;
        }
    }

    index = 0;
    while((index = entityParser.indexIn(*str,index)) != -1)
    {
        index++;
        decodedChar = resolveEntity(entityParser.cap(2));
        qDebug() << "Decoding in " << entityParser.cap(1) << ":" << entityParser.cap(2) << " to " << decodedChar << "(" << decodedChar.isNull()<<")";
        if (!decodedChar.isNull()) {
            qDebug() << "Replacing in " << entityParser.cap(1) << ":" << entityParser.cap(2) << " to " << decodedChar;
            str->replace(entityParser.cap(1),QString(decodedChar));
        }
    }

}



// Decode une chaine de caractere utilisant des
// caracteres speciaux d'HTML.

QString HTML::decode(const QString* src)
{
    QString str(*src);
    decodeStr(&str);
    return str;
}



// Decode une chaine de caractere utilisant des
// caracteres speciaux d'HTML.

QString HTML::decode(const QString& src)
{
    QString str(src);
    decodeStr(&str);
    return str;
}



// Decode une chaine de caractere utilisant des
// caracteres speciaux d'HTML.

QString HTML::decode(const char* src)
{
    QString str(src);
    decodeStr(&str);
    return str;
}


// ------------------------------------------------------------------
//                      Fonction d'encodage
// ------------------------------------------------------------------

// Encode une chaine de caractere utilisant des
// caracteres speciaux d'HTML.

void HTML::encodeStr(QString* str)
{
    QChar c;
    QString encodedChar;

    for(int pos=0; pos<str->size(); pos++)
    {
        c = str->at(pos);
        encodedChar = resolveEntity(c.unicode());
        if (!encodedChar.isNull())
        {
            str->remove(pos, 1);
            str->insert(pos, QString("&%1;").arg(encodedChar.toLower()));
            pos += encodedChar.size() + 2;
        }
    }
}




// Encode une chaine de caractere utilisant des
// caracteres speciaux d'HTML.

QString HTML::encode(const QString* src)
{
    QString str(*src);
    encodeStr(&str);
    return str;
}



// Encode une chaine de caractere utilisant des
// caracteres speciaux d'HTML.

QString HTML::encode(const QString& src)
{
    QString str(src);
    encodeStr(&str);
    return str;
}





// Encode une chaine de caractere utilisant des
// caracteres speciaux d'HTML.

QString HTML::encode(const char* src)
{
    QString str(src);
    encodeStr(&str);
    return str;
}
