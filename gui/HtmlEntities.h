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

#ifndef HTMLENTITIES_H
#define HTMLENTITIES_H

#include <QtCore/QString>

struct HtmlEntity
{
  const char *name;
  quint16 code;
};


class HTML
{
  public :
    
    // Decode une chaine de caractere utilisant des
    // caracteres speciaux d'HTML.

    static void decodeStr(QString* str);
    static QString decode(const QString* src);
    static QString decode(const QString& src);
    static QString decode(const char* src);

    // Encode une chaine de caractere utilisant des
    // caracteres speciaux d'HTML.
  
    static void encodeStr(QString* str);
    static QString encode(const QString* src);
    static QString encode(const QString& src);
    static QString encode(const char* src);

    // Retourne le caractere associe a un caractere special HTML.
  
    static QChar resolveEntity(const QString &entity);
  
  
    // Retourne la chaine HTML representant le caractere "entityCode".
  
    static QString resolveEntity(quint16 entityCode);
    
    
  private :
    
    static const int MaxEntities;
    static const int FirstEntities;
    static const HtmlEntity first[];
    static const HtmlEntity entities[];
    
    static const HtmlEntity* end_ent;
    static const HtmlEntity* start_ent;
    static const HtmlEntity* end_first;
    static const HtmlEntity* start_first;

};

#endif
