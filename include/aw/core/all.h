#pragma once

#include "aw/core/misc/aw_core_checks.h"

#include "aw/core/primitive/numbers.h"
#include "aw/core/primitive/container_aliases.h"
#include "aw/core/primitive/defer.h"
#include "aw/core/primitive/macros.h"
#include "aw/core/primitive/enum_flags.h"

#include "aw/core/memory/memalloc.h"
#include "aw/core/memory/allocators.h"
#include "aw/core/memory/paged_memory_pool.h"
#include "aw/core/memory/intrusive_ref_counted.h"

#include "aw/core/math/math.h"
#include "aw/core/math/vector2.h"
#include "aw/core/math/vector3.h"
#include "aw/core/math/vector4.h"
#include "aw/core/math/matrix.h"

#include "aw/core/async/thread_pool.h"
#include "aw/core/async/thread_worker.h"
#include "aw/core/async/async.h"
#include "aw/core/async/task_graph.h"

#include "aw/core/filesystem/file.h"