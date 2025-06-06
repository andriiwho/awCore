#pragma once

#include "aw/core/misc/aw_core_checks.h"

#include "primitive/numbers.h"
#include "primitive/container_aliases.h"
#include "primitive/defer.h"
#include "primitive/macros.h"

#include "memory/memalloc.h"
#include "memory/allocators.h"
#include "memory/paged_memory_pool.h"
#include "memory/intrusive_ref_counted.h"

#include "aw/core/math/math.h"
#include "aw/core/math/vector2.h"
#include "aw/core/math/vector3.h"
#include "aw/core/math/vector4.h"
#include "aw/core/math/matrix.h"

#include "aw/core/async/thread_pool.h"
#include "aw/core/async/thread_worker.h"
#include "aw/core/async/async.h"
#include "aw/core/async/task_graph.h"