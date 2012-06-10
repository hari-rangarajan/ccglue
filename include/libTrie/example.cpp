/*
** example.cpp
** Login : <speedblue@happycoders.org>
** Started on  Sun Dec  3 18:40:42 2006 Julien Lemoine
** $Id$
** 
** Copyright (C) 2006 Julien Lemoine
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#include "src/Trie.hxx"
#include "src/Exception.h"

int main(int argc, char **argv)
{
  /// trie that associates a integer to strings
  /// 0 is the default value I want to receive when there is no match
  /// in trie
  ToolBox::Trie<unsigned>	trie(0);

  std::cout << "set [First String] = 1" << std::endl;
  trie.addEntry("First String", 12, 1);
  std::cout << "set [Second String] = 2" << std::endl;
  trie.addEntry("Second String", 13, 2);
  std::cout << "set [First Element] = 3" << std::endl;
  trie.addEntry("First Element", 13, 3);
  std::cout << "set [Second Element] = 4" << std::endl;
  trie.addEntry("Second Element", 14, 4);

  std::cout << "get [First String] = " << trie.getEntry("First String", 12) << std::endl;
  std::cout << "get [Second String] = " << trie.getEntry("Second String", 13) << std::endl;
  std::cout << "get [First Element] = " << trie.getEntry("First Element", 13) << std::endl;
  std::cout << "get [Second Element] = " << trie.getEntry("Second Element", 14) << std::endl;

  std::cout << "set [First String] = 10" << std::endl;
  trie.setEntry("First String", 12, 10);
  std::cout << "get [First String] = " << trie.getEntry("First String", 12) << std::endl;

  // no match
  std::cout << "get [No match for this entry] = " << trie.getEntry("No match for this entry", 23) << std::endl;
}
