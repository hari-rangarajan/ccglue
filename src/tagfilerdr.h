#ifndef TAGFILERDR_H
#define TAGFILERDR_H

#include <list>
#include <istream>
#include <sstream>
#include <iterator>
#include <string>


#include "lexertl/rules.hpp"
#include "lexertl/state_machine.hpp"
#include "lexertl/generator.hpp"
#include "lexertl/lookup.hpp"


#include "digraph.h"
#include "tracer.h"
#include "tag_db.h"

#define MAX_SIZE_LINE_NUMBER   256
#define MAX_SIZE_TAG_NAME   256
#define MAX_SIZE_TAG_FILE_NAME 512
#define MAX_SIZE_TAG_LOCATION 1
#define MAX_SIZE_TAG_LINE 4096

#endif
