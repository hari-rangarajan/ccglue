/*
** TestSuite.cpp
** Login : <speedblue@happycoders.org>
** Started on  Thu Jul 13 22:06:54 2006 Julien Lemoine
** $Id$
** 
** Copyright (C) 2006 Julien Lemoine
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
** 
** You should have received a copy of the GNU Lesser General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#include <iostream>
#include <cppunit/TestSuite.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/TestResult.h>
#include <cppunit/TextTestProgressListener.h>

#include "TestToolBoxTrie.h"

class OutputTestNameListener : public CppUnit::TextTestProgressListener
{
  void startTest(CppUnit::Test *test)
  {
	std::cerr << "Running " << test->getName() << std::endl;
	std::cerr.flush();
  }
};

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(UnitTest::TestToolBoxTrie, "tests" );

int main(int argc, char **argv)
{
  CppUnit::TestSuite		*suite = new CppUnit::TestSuite("Test Suite");
  CppUnit::TextUi::TestRunner	runner;

  suite->addTest(CppUnit::TestFactoryRegistry::getRegistry("tests").makeTest());
  runner.addTest(suite);
  OutputTestNameListener listener;
  runner.eventManager().addListener(&listener);

  bool success = runner.run();

  return (success ? 0 : 1);
}
