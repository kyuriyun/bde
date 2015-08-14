// bdlb_variant.h                                                    -*-C++-*-
#ifndef INCLUDED_BDLB_VARIANT
#define INCLUDED_BDLB_VARIANT

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a variant (discriminated 'union'-like) type.
//
//@CLASSES:
//    bdlb::Variant:    variant of up to 20 types
//    bdlb::Variant2:   variant of exactly 2 types
//    bdlb::Variant3:   variant of exactly 3 types
//    bdlb::Variant4:   variant of exactly 4 types
//    bdlb::Variant5:   variant of exactly 5 types
//    bdlb::Variant6:   variant of exactly 6 types
//    bdlb::Variant7:   variant of exactly 7 types
//    bdlb::Variant8:   variant of exactly 8 types
//    bdlb::Variant9:   variant of exactly 9 types
//    bdlb::Variant10:  variant of exactly 10 types
//    bdlb::Variant11:  variant of exactly 11 types
//    bdlb::Variant12:  variant of exactly 12 types
//    bdlb::Variant13:  variant of exactly 13 types
//    bdlb::Variant14:  variant of exactly 14 types
//    bdlb::Variant15:  variant of exactly 15 types
//    bdlb::Variant16:  variant of exactly 16 types
//    bdlb::Variant17:  variant of exactly 17 types
//    bdlb::Variant18:  variant of exactly 18 types
//    bdlb::Variant19:  variant of exactly 19 types
//    bdlb::VariantImp: variant from a type list
//
//@SEE_ALSO:
//
//@AUTHOR: Ilougino Rocha (irocha), Arthur Chiu (achiu21)
//
//@DESCRIPTION: This component provides a variant class, 'bdlb::Variant', that
// can store an instance of one of up to some (implementation-defined) number
// of parameterizable types (currently 20).  A variant can hold any one of the
// types defined in its signature at any one point of time.  Clients can
// retrieve the value and type that a variant currently holds, assign new
// values to the variant or apply a visitor to a variant, which acts based on
// the value and type the variant currently holds.  Assigning a value of a new
// type destroys the object of the old type and constructs the new value by
// copy constructing the supplied value.
//
// When the number ('N') of types that need to be supported is known, it is
// better to use the 'bdeut::VariantN' templates, which use an identical
// implementation, but generate shorter symbols and debugging information due
// to the lack of defaulted template argument types.  Note that
// 'bdeut::VariantN<T1, ..., TN>' and 'bdlb::Variant<T1, ..., TN>',
// nevertheless, are distinct types.
//
// When the variant types are (directly) supplied as a type list (of type
// 'bslmf::TypeList'), the type 'bdlb::VariantImp<TYPELIST>' can be used in
// place of:
//..
//  bdlb::Variant<typedef TYPELIST::Type1, typedef TYPELIST::Type2, ...>
//..
//
///Default Construction
///--------------------
// The 'bdlb::Variant' class, when default constructed, will not hold a value
// nor type, and 'isUnset' will return 'true'.  This state is the same as a
// 'bdlb::Variant' that had been reset by the 'reset' method.
//
///Visitor
///-------
// 'bdlb::Variant' provides an 'apply' method that implements the visitor
// design pattern.  'apply' accepts a visitor (functor) that provides an
// 'operator()', which will be invoked with the value that the variant
// currently holds.
//
// The 'apply' method should be preferred over a 'switch'-statement based on
// the type index of a variant.  If the order or types contained by the variant
// is changed in the future, every place where the type index is hard-coded
// needs to be updated.  Whereas, if 'apply' were used, no change would be
// needed because function overloading should automatically resolve to the
// proper 'operator()' to invoke.
//
// There are several variations of the 'apply' method, varying based on the
// return type of 'operator()' and the handling of unset variants.  Firstly,
// the method varies based on whether 'operator()' returns a value or not.
// There can either be:
//..
//  a) No return value
//  b) A return type specified in the visitor interface
//  c) A return type specified explicitly when invoking 'apply'
//..
// The default is case (a) - no return value.  If users would like to return
// a value from the visitor's 'operator()', the functor should specify an alias
// 'ResultType' to the desired return type.  For example, if 'operator()' were
// to return an 'int', the functor class should specify:
//..
//  typedef int ResultType;
//..
// If 'ResultType' cannot be determined, users also have the option of
// explicitly specifying the return type when invoking apply:
//..
//  apply<int>(visitor);
//..
// Secondly, the 'apply' method varies based on how the 'apply' method handles
// an unset variant.  A users can choose to either:
//..
//  a) Pass a default constructed 'bslmf::Nil' to the visitor
//  b) Pass a user specified "default" value to the visitor
//..
// Furthermore, if the user is sure that the variant cannot be unset, the user
// can invoke 'applyRaw', which is slightly more efficient, but results in
// undefined behavior if the variant is, in fact, unset.
//
///'bdex' Streamability
///--------------------
// The 'bdlb::Variant' class can be streamed using the 'bdexStreamIn' and
// 'bdexStreamOut' methods.  That is, if the version number with which the
// variant type is streamed is known by both parties, there will be no problem.
//
// Note that this version number must be known by the reader independently of
// the actual value of the object streamed.  An "adaptive" version number such
// as: 1 if the variant is a 'T1', 2 if it is a 'T2', etc., will make the
// 'bdlb::Variant' out-streamable, but *not* in-streamable since the reader has
// no idea of the value (or its type) before beginning streaming.  In general,
// a common version number accepted by all types (e.g., 1) will work.
//
// There is, however, one important restriction: 'bdex' streaming will
// generally *not* work correctly as a *top*-*level* object.  That is, one
// cannot apply a stream 'operator>>' to stream in a value.  To see why,
// consider that such operators must be implemented in terms of:
//..
//  'bdex_InStreamFunctions::streamInVersionAndObject'
//..
// But 'streamInVersionAndObject' is in a quandary, since it must determine
// whether a version number is streamed or not, and this depends on the actual
// streamed value (which is at that time not yet known).
//
// There are two situations in which a variant type with 'N' explicit type
// arguments, 'bdeut::VariantN<T1, ..., TN>', is streamable as a top-level
// object:
//..
//  1) If *none* of the types 'T1', ..., 'TN' use a version number, or
//
//  2) If *all* of the types 'T1', ..., 'TN' use a version number, and the
//     current value of the variant is set (i.e., its type index is not 0).
//..
// In either case, the current maximum supported version number of the variant
// will be either positive (indicating that a version number must be streamed),
// or not (indicating that no version number is streamed), which will be
// compatible with the value that is subsequently streamed in.
//
///Usage
///-----
///Example 1: Variant Construction
///- - - - - - - - - - - - - - - -
// The following example illustrates the different ways of constructing a
// 'bdlb::Variant':
//..
//  typedef bdlb::Variant <int, double, bsl::string> List;
//  typedef bdlb::Variant3<int, double, bsl::string> List3;  // equivalent
//..
// The contained types can be retrieved as a 'bslmf::TypeList' (using the
// 'TypeList' nested type), or individually (using 'TypeN', for 'N' varying
// from 1 to the length of the 'TypeList').  In the example below, we use the
// 'List' variant, but this could be substituted with 'List3' with no change
// to the source code:
//..
//  assert(3 == List::TypeList::LENGTH);
//..
// We can check whether the variant defaults to the unset state by using the
// 'is<TYPE>' and 'typeIndex' methods:
//..
//  List x;
//
//  assert(!x.is<int>());
//  assert(!x.is<double>());
//  assert(!x.is<bsl::string>());
//  assert(0 == x.typeIndex());
//..
// Single argument construction from a type in the typelist to a variant is
// also supported.  This is more efficient than creating an unset variant and
// assigning to it a value:
//..
//  List y(bsl::string("Hello"));
//
//  assert(!y.is<int>());
//  assert(!y.is<double>());
//  assert( y.is<bsl::string>());
//
//  assert("Hello" == y.the<bsl::string>());
//..
// Furthermore, 'createInPlace' is provided to support direct in place
// construction.  This method allows users to directly construct the target
// type inside the variant, instead of first creating a temporary object, then
// copy construct the object to initialize the variant:
//..
//  List z;
//  z.createInPlace<bsl::string>("Hello", 5);
//
//  assert(!z.is<int>());
//  assert(!z.is<double>());
//  assert( z.is<bsl::string>());
//
//  assert("Hello" == z.the<bsl::string>());
//..
// Up to 14 constructor arguments are supported for in-place construction of
// an object.  Users can also safely create another object of the same or
// different type in a variant that already holds a value using the
// 'createInPlace' method.  No memory will be leaked in all cases and
// destructors for the currently held object will be invoked:
//..
//  z.createInPlace<bsl::string>("Hello", 5);
//  assert(z.is<bsl::string>());
//  assert("Hello" == z.the<bsl::string>());
//
//  z.createInPlace<double>(10.0);
//  assert(z.is<double>());
//  assert(10.0 == z.the<double>());
//
//  z.createInPlace<int>(10);
//  assert(z.is<int>());
//  assert(10 == z.the<int>());
//..
//
///Example 2: Variant Assignment
///- - - - - - - - - - - - - - -
// A value of a given type can be stored in a variant in three different ways:
//..
//  a) 'operator='
//  b) 'assignTo<TYPE>'
//  c) 'assign'
//..
// 'operator=' will automatically deduce the type that the user is trying to
// assign to the variant.  This should be used most of the time.  The
// 'assignTo<TYPE>' method should be used when conversion to the type that
// the user is assigning to is necessary (see example 2a and 2b for more
// details).  Finally, 'assign' is equivalent to 'operator=' and exists simply
// for backwards compatibility.
//
///a) 'operator='
///-  -  -  -  -
// The following example illustrates how to use 'operator=':
//..
//  typedef bdlb::Variant <int, double, bsl::string> List;
//
//  List x;
//
//  List::Type1 v1 = 1;       // 'int'
//  List::Type2 v2 = 2.0;     // 'double'
//  List::Type3 v3("hello");  // 'bsl::string'
//
//  x = v1;
//  assert(x.is<int>());
//  assert(!x.is<double>());
//  assert(!x.is<bsl::string>());
//  assert(v1 == x.the<int>());
//
//  x = v2;
//  assert(!x.is<int>());
//  assert( x.is<double>());
//  assert(!x.is<bsl::string>());
//  assert(v2 == x.the<double>());
//
//  x = v3;
//  assert(!x.is<int>());
//  assert(!x.is<double>());
//  assert( x.is<bsl::string>());
//  assert(v3 == x.the<bsl::string>());
//..
// Note that the type of the object can be deduced automatically during
// assignment, as in:
//..
//  x = v1;
//..
// This automatic deduction, however, cannot be extended to conversion
// constructors, such as:
//..
//  x = (const char *)"hello";                // ERROR
//..
// The compiler will complain that 'const char *' is not a variant type
// specified in the list of parameter types used in the definition of 'List',
// and will trigger a compile-time assertion.  To overcome this problem, see
// the next usage example on 'assignTo<TYPE>':
//
///b) 'assignTo<TYPE>'
///-  -  -  -  -  -  -
// In example 2a, 'const char *' is not part of the variant's typelist, which
// resulted in a compilation error.  The use of 'assignTo<TYPE>' explicitly
// informs the compiler of the intended type to assign to the variant:
//..
//  x.assignTo<bsl::string>((const char*)"hello");
//..
//
///c) 'assign'
///-  -  -  -  -  -
// Finally, for backwards compatibility reasons, 'assign' can also be used in
// place of 'operator=' (but not 'assignTo'):
//..
//  x.assign<int>(v1);
//  assert( x.is<int>());
//  assert(!x.is<double>());
//  assert(!x.is<bsl::string>());
//  assert(v1 == x.the<int>());
//
//  x.assign<double>(v2);
//  assert(!x.is<int>());
//  assert( x.is<double>());
//  assert(!x.is<bsl::string>());
//  assert(v2 == x.the<double>());
//
//  x.assign<bsl::string>(v3);
//  assert(!x.is<int>());
//  assert(!x.is<double>());
//  assert( x.is<bsl::string>());
//  assert(v3 == x.the<bsl::string>());
//..
//
///Example 3: Visiting variant with 'apply'
///- - - - - - - - - - - - - - - - - - - - -
// As described in the "visitors" section, there are different ways to invoke
// the 'apply' method.  The first three usage examples illustrate the
// different ways to invoke 'apply' (with no return value) to control the
// behavior of visiting an unset variant:
//..
//  a) 'bslmf::Nil' passed to visitor
//
//  b) user specified default value passed to visitor
//
//  c) undefined behavior if visitor is unset
//..
// Then, the next two usage examples illustrate different ways to specify the
// return value from 'apply:
//..
//  d) return value specified in visitor
//
//  e) return value specified with function call
//..
//
///a) 'bslmf::Nil' passed to visitor
///-  -  -  -  -  -  -  -  -  -  - -
// A simple visitor that does not require any return values might be one that
// prints to 'stdout' the value of the variant:
//..
//  class my_PrintVisitor
//  {
//    public:
//      template <class TYPE>
//      void operator()(const TYPE& value) const
//      {
//          bsl::cout << value << bsl::endl;
//      }
//
//      void operator()(bslmf::Nil value) const
//      {
//          bsl::cout << "null" << bsl::endl;
//      }
//  };
//
//  typedef bdlb::Variant <int, double, bsl::string> List;
//
//  List x[4];
//
//      //************************************
//      // Note that 'x[3]' is uninitialized *
//      //************************************
//
//  x[0].assign(1);
//  x[1].assign(1.1);
//  x[2].assignTo<bsl::string>((const char *)"Hello");
//
//  my_PrintVisitor printVisitor;
//
//  for (int i = 0; i < 4; ++i) {
//      x[i].apply(printVisitor);
//  }
//..
// The above will print the following on 'stdout':
//..
//  1
//  1.1
//  Hello
//  null
//..
// Note that 'operator()' is overloaded with 'bslmf::Nil'.  A direct match has
// higher precedence than a template parameter match.  When the variant is
// unset (such as 'x[3]'), a 'bslmf::Nil' will be passed to the visitor.
//
///b) User specified default value passed to visitor
///-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
// Instead of using 'bslmf::Nil', users can also specify a default value to
// pass to the visitor when the variant is currently unset.  Using the same
// 'my_PrintVisitor' class from example 3a:
//..
//  for (int i = 0; i < 4; ++i) {
//      x[i].apply(printVisitor, "Print this when unset");
//  }
//..
// Now, the above code will print the following on 'stdout':
//..
//  1
//  1.1
//  Hello
//  Print this when unset
//..
// This variation of 'apply' is useful since the user can provide a default
// value to the variant without incurring the cost of initializing the variant
// itself.
//
///c) Undefined behavior if visitor is unset
///-  -  -  -  -  -  -  -  -  -  -  -  -  -  -
// If the user is sure that their variant type will never be unset, the
// 'applyRaw' method can be used instead.  This method is slightly more
// efficient than the regular 'apply' method, but the behavior is undefined if
// the variant is currently unset.  Again, borrowing 'my_PrintVisitor' from
// example 3a:
//..
//  typedef bdlb::Variant <int, double, bsl::string> List;
//
//  List x[4];
//
//      //************************************
//      // Note that 'x[3]' is uninitialized *
//      //************************************
//
//  x[0].assign(1);
//  x[1].assign(1.1);
//  x[2].assignTo<bsl::string>((const char *)"Hello");
//
//  my_PrintVisitor printVisitor;
//
//  for (int i = 0; i < 4; ++i) {
//      x[i].applyRaw(printVisitor);  // undefined behavior for 'x[3]'
//  }
//..
//
///d) Return value specified in visitor
///-  -  -  -  -  -  -  -  -  -  -  -  -
// Users can also specify a return type that 'operator()' will return by
// specifying a 'typedef' with the name 'ResultType' in their functor class.
// This is necessary in order for the 'apply' method to know what type to
// return at compile time:
//..
//  class my_AddVisitor
//  {
//    public:
//      typedef bool ResultType;
//
//      //************************************************************
//      // Note that the return type of 'operator()' is 'ResultType' *
//      //************************************************************
//
//      template <class TYPE>
//      ResultType operator()(TYPE& value) const
//          // Return 'true' when addition is performed successfully and
//          // 'false' otherwise.
//      {
//          if (bslmf::IsConvertible<TYPE, double>::VALUE) {
//
//              // Add certain values to the variant.  The details are elided
//              // as it is the return value that is the focus of this example.
//
//              return true;
//          }
//          return false;
//      }
//  };
//
//  typedef bdlb::Variant <int, double, bsl::string> List;
//
//  List x[3];
//
//  x[0].assign(1);
//  x[1].assign(1.1);
//  x[2].assignTo<bsl::string>((const char *)"Hello");
//
//  my_AddVisitor addVisitor;
//
//  bool ret[3];
//
//  for (int i = 0; i < 3; ++i) {
//      ret[i] = x[i].apply(addVisitor);
//      if (!ret[i]) {
//          bsl::cout << "Cannot add to types not convertible to double."
//                    << bsl::endl;
//      }
//  }
//  ASSERT(true  == ret[0]);
//  ASSERT(true  == ret[1]);
//  ASSERT(false == ret[2]);
//..
// The above will print the following on 'stdout':
//..
//  Cannot add to types not convertible to double.
//..
// Note that if no 'typedef' is provided (as in the 'my_PrintVisitor' in
// example 3a), then the default return value is 'void'.
//
///e) Return value specified with function call
///-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
// There may be some cases when a visitor interface is not owned by a
// client (hence the client cannot add a 'typedef' to the visitor), or the
// visitor could not determine the return type at design time.  In these
// scenarios, users can explicitly specify the return type when invoking
// 'visit':
//..
//  class ThirdPartyVisitor
//  {
//    public:
//      template <class TYPE>
//      bsl::string operator()(const TYPE& value) const;
//          // Returns the name of the specified 'value' as a 'bsl::string'.
//          // Note that the implementation of this class is deliberately not
//          // shown since this class belongs to a third party library.
//  };
//
//  typedef bdlb::Variant <int, double, bsl::string> List;
//
//  List x[3];
//
//  x[0].assign(1);
//  x[1].assign(1.1);
//  x[2].assignTo<bsl::string>((const char *)"Hello");
//
//  ThirdPartyVisitor visitor;
//
//  for (int i = 0; i < 3; ++i) {
//
//      //****************************************************
//      // Note that the return type is explicitly specified *
//      //****************************************************
//
//      bsl::string ret = x[i].apply<bsl::string>(visitor);
//      bsl::cout << ret << bsl::endl;
//  }
//..
//
///Class Synopsis
///--------------
// Due to the complexity of the implementation, the following synopsis is
// provided to aid users in locating documentation for functions.  Note that
// this is not a complete summary of all available methods.  Only the key
// methods are shown.  For more information, please refer to the function
// level documentation:
//
///Creators
///- - - - -
//..
//  bdlb::Variant()
//  bdlb::Variant(const TYPE_OR_ALLOCATOR& typeOrAllocator);
//  bdlb::Variant(const TYPE&       value,
//                bslma::Allocator *basicAllocator);
//..
// Create a variant.  Users can choose to initialize this variant with a
// specified value, or leave this variant in an unset state from default
// construction.  Users can also supply a 'bslma::Allocator' for memory
// allocation.
//
///Manipulators
///- - - - - - -
//..
//  bdlb::Variant& operator=(const TYPE& value);
//..
// Assign a different value of templatized 'TYPE' to this variant.
//..
//  bdlb::Variant& operator=(const bdlb::Variant& rhs);
//..
// Assign another variant to this variant.
//..
//  void                apply(VISITOR& visitor);
//  VISITOR::ResultType apply(VISITOR& visitor);
//  RET_TYPE            apply(VISITOR& visitor);
//..
// Access this variant's value using the specified visitor functor (that passes
// 'bslmf::Nil' to the visitor's 'operator()' when the visitor is unset).
//..
//  void                apply(VISITOR& visitor, const TYPE& dV);
//  VISITOR::ResultType apply(VISITOR& visitor, const TYPE& dV);
//  RET_TYPE            apply(VISITOR& visitor, const TYPE& dV);
//..
// Access this variant's value using the specified visitor functor (that passes
// user specified default value 'dV' to the visitor's 'operator()' when the
// visitor is unset).
//..
//  void                applyRaw(VISITOR& visitor);
//  VISITOR::ResultType applyRaw(VISITOR& visitor);
//  RET_TYPE            applyRaw(VISITOR& visitor);
//..
// Access this variant's value using the specified visitor functor (where
// behavior is undefined when this variant is unset).
//..
//  template <class TYPE>
//  void createInPlace();
//  void createInPlace(const A1& a1);
//  // ...
//  void createInPlace(const A1& a1, const A2& a2, ..., const A14& a14);
//..
// Create a new value of templatized 'TYPE' in place, with up to 14 constructor
// arguments.
//..
//  void reset();
//..
// Reset this variant to an unset state.
//..
//  template <class TYPE>
//  TYPE& the();
//..
// Access the value of parameterized 'TYPE' currently held by this variant.
// This method should be invoked with the syntax 'the<TYPE>()', e.g.,
// 'the<int>()'.
//
///Accessors
///- - - - -
//..
//  template <class TYPE>
//  bool is() const;
//..
// Check whether this variant is currently holding a particular type.  This
// method should be invoked with the syntax 'is<TYPE>()', e.g., 'is<int>()'.
//..
//  bool isUnset() const;
//..
// Return 'true' if this variant is currently unset, and 'false' otherwise.
//..
//  bsl::ostream& print(bsl::ostream& stream,
//                      int           level          = 0,
//                      int           spacesPerLevel = 4) const;
//..
// Write a description of this variant to the specified 'stream'.

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLB_PRINTMETHODS
#include <bdlb_printmethods.h>
#endif

#ifndef INCLUDED_BDLXXXX_INSTREAMFUNCTIONS
#include <bdlxxxx_instreamfunctions.h>
#endif

#ifndef INCLUDED_BDLXXXX_OUTSTREAMFUNCTIONS
#include <bdlxxxx_outstreamfunctions.h>
#endif

#ifndef INCLUDED_BDLXXXX_VERSIONFUNCTIONS
#include <bdlxxxx_versionfunctions.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLALG_SCALARDESTRUCTIONPRIMITIVES
#include <bslalg_scalardestructionprimitives.h>
#endif

#ifndef INCLUDED_BSLALG_SCALARPRIMITIVES
#include <bslalg_scalarprimitives.h>
#endif

#ifndef INCLUDED_BSLALG_SWAPUTIL
#include <bslalg_swaputil.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSLMF_ASSERT
#include <bslmf_assert.h>
#endif

#ifndef INCLUDED_BSLMF_ENABLEIF
#include <bslmf_enableif.h>
#endif

#ifndef INCLUDED_BSLMF_ISCLASS
#include <bslmf_isclass.h>
#endif

#ifndef INCLUDED_BSLMF_ISSAME
#include <bslmf_issame.h>
#endif

#ifndef INCLUDED_BSLMF_NIL
#include <bslmf_nil.h>
#endif

#ifndef INCLUDED_BSLMF_TYPELIST
#include <bslmf_typelist.h>
#endif

#ifndef INCLUDED_BSLS_ALIGNMENT
#include <bsls_alignment.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSL_ALGORITHM
#include <bsl_algorithm.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_TYPEINFO
#include <bsl_typeinfo.h>
#endif

namespace BloombergLP {


namespace bdlb {struct Variant_DefaultConstructVisitor;
struct Variant_CopyConstructVisitor;
struct Variant_DestructorVisitor;
struct Variant_AssignVisitor;
struct Variant_MaxSupportedBdexVersionVisitor;
struct Variant_PrintVisitor;
struct Variant_EqualityTestVisitor;

template <class STREAM>
struct Variant_BdexStreamInVisitor;
template <class STREAM>
struct Variant_BdexStreamOutVisitor;

template <typename TYPES>
class VariantImp;

                    // ======================================
                    // struct Variant_ReturnValueHelper
                    // ======================================

// These definitions provided outside of 'Variant_ReturnValueHelper'
// because of a bug in IBM xlC compiler, DRQS 37714216.
typedef char                  Variant_ReturnValueHelper_YesType;
}  // close package namespace
typedef struct { char a[2]; } Variant_ReturnValueHelper_NoType;

BSLMF_ASSERT(sizeof(bdlb::Variant_ReturnValueHelper_YesType)
             != sizeof(Variant_ReturnValueHelper_NoType));

namespace bdlb {
template <class VISITOR>
struct Variant_ReturnValueHelper {
    // This struct is a component-private meta-function.  Do *not* use.  This
    // meta-function checks whether the parameterized 'VISITOR' type has the
    // member 'ResultType' defined using "SFINAE" - substitution failure is not
    // an error.

    template <typename T> static Variant_ReturnValueHelper_YesType
        match(typename T::ResultType*);
    template <typename T> static Variant_ReturnValueHelper_NoType
        match(...);
        // If 'T::Type' exists, then the first function will be a better match
        // than the ellipsis version, which will return a 'YesType', indicating
        // the existence of 'T::ResultType'.

    enum {
        k_VaL = sizeof(match<VISITOR>(0))
                   == sizeof(Variant_ReturnValueHelper_YesType)
    };
        // A 'k_VaL' of 'true' indicates 'VISITOR::ResultType' exists, and
        // 'false' otherwise.
};

                   // ===========================================
                   // class VariantImp_AllocatorBase<TYPES>
                   // ===========================================

template <class TYPES>
class VariantImp_AllocatorBase {
    // This class is component-private.  Do not use.  This class contains the
    // 'typedef's and data members of the 'Variant' class.  This class
    // serves as a base class for the variant when any one of the types held by
    // the variant has the 'bslalg::TypeTraitUsesBslmaAllocator' type trait.

  public:
    // TYPES
    typedef TYPES TypeList;
        // 'TypeList' is an alias for the 'bslmf::TypeList' type serving as the
        // template parameter to this variant implementation.

    typedef typename bslmf::TypeListTypeOf< 1, TYPES>::TypeOrDefault Type1;
    typedef typename bslmf::TypeListTypeOf< 2, TYPES>::TypeOrDefault Type2;
    typedef typename bslmf::TypeListTypeOf< 3, TYPES>::TypeOrDefault Type3;
    typedef typename bslmf::TypeListTypeOf< 4, TYPES>::TypeOrDefault Type4;
    typedef typename bslmf::TypeListTypeOf< 5, TYPES>::TypeOrDefault Type5;
    typedef typename bslmf::TypeListTypeOf< 6, TYPES>::TypeOrDefault Type6;
    typedef typename bslmf::TypeListTypeOf< 7, TYPES>::TypeOrDefault Type7;
    typedef typename bslmf::TypeListTypeOf< 8, TYPES>::TypeOrDefault Type8;
    typedef typename bslmf::TypeListTypeOf< 9, TYPES>::TypeOrDefault Type9;
    typedef typename bslmf::TypeListTypeOf<10, TYPES>::TypeOrDefault Type10;
    typedef typename bslmf::TypeListTypeOf<11, TYPES>::TypeOrDefault Type11;
    typedef typename bslmf::TypeListTypeOf<12, TYPES>::TypeOrDefault Type12;
    typedef typename bslmf::TypeListTypeOf<13, TYPES>::TypeOrDefault Type13;
    typedef typename bslmf::TypeListTypeOf<14, TYPES>::TypeOrDefault Type14;
    typedef typename bslmf::TypeListTypeOf<15, TYPES>::TypeOrDefault Type15;
    typedef typename bslmf::TypeListTypeOf<16, TYPES>::TypeOrDefault Type16;
    typedef typename bslmf::TypeListTypeOf<17, TYPES>::TypeOrDefault Type17;
    typedef typename bslmf::TypeListTypeOf<18, TYPES>::TypeOrDefault Type18;
    typedef typename bslmf::TypeListTypeOf<19, TYPES>::TypeOrDefault Type19;
    typedef typename bslmf::TypeListTypeOf<20, TYPES>::TypeOrDefault Type20;
        // 'TypeN' is an alias for the 'N'th type in the 'TypeList' of this
        // variant implementation.  If less than 'N' template arguments were
        // given to the 'Variant' type, then 'TypeN' is
        // 'bslmf::Nil'.

  private:
    union Value {
        bsls::ObjectBuffer<Type1>       d_v1;
        bsls::ObjectBuffer<Type2>       d_v2;
        bsls::ObjectBuffer<Type3>       d_v3;
        bsls::ObjectBuffer<Type4>       d_v4;
        bsls::ObjectBuffer<Type5>       d_v5;
        bsls::ObjectBuffer<Type6>       d_v6;
        bsls::ObjectBuffer<Type7>       d_v7;
        bsls::ObjectBuffer<Type8>       d_v8;
        bsls::ObjectBuffer<Type9>       d_v9;
        bsls::ObjectBuffer<Type10>      d_v10;
        bsls::ObjectBuffer<Type11>      d_v11;
        bsls::ObjectBuffer<Type12>      d_v12;
        bsls::ObjectBuffer<Type13>      d_v13;
        bsls::ObjectBuffer<Type14>      d_v14;
        bsls::ObjectBuffer<Type15>      d_v15;
        bsls::ObjectBuffer<Type16>      d_v16;
        bsls::ObjectBuffer<Type17>      d_v17;
        bsls::ObjectBuffer<Type18>      d_v18;
        bsls::ObjectBuffer<Type19>      d_v19;
        bsls::ObjectBuffer<Type20>      d_v20;
    };
        // 'Value' is a union of 'bsls::ObjectBuffer' of all types contained by
        // the variant.  'bsls::ObjectBuffer' is used to:  1) Wrap around non
        // "POD" types within the union.  2) Ensure proper alignment of the
        // type.

    // DATA
    Value             d_value;        // value of the object, initialized by
                                      // child class

    int               d_type;         // current type the variant is holding

    bslma::Allocator *d_allocator_p;  // pointer to allocator (held, not owned)

    // FRIENDS
    template <typename VARIANT_TYPES>
    friend class VariantImp;

    template <typename VARIANT_TYPES>
    friend bool operator==(const VariantImp<VARIANT_TYPES>&,
                           const VariantImp<VARIANT_TYPES>&);

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(VariantImp_AllocatorBase,
                                 bslalg::TypeTraitUsesBslmaAllocator);

    // CREATORS
    VariantImp_AllocatorBase(int type, bslma::Allocator *basicAllocator);

    VariantImp_AllocatorBase(int                type,
                                   bslma::Allocator  *basicAllocator,
                                   bslmf::MetaInt<1> *);
        // Create a 'VariantImp_AllocatorBase' with the specified 'type'
        // indicating the type the variant is currently holding, and the
        // specified 'basicAllocator' to supply memory.

    template <typename TYPE>
    VariantImp_AllocatorBase(int type, const TYPE&, bslmf::MetaInt<0> *);

    // ACCESSORS
    bslma::Allocator *getAllocator() const;
        // Return the allocator pointer currently held by this object.
};

                  // ==============================================
                  // class VariantImp_NonAllocatorBase<TYPES>
                  // ==============================================

template <typename TYPES>
class VariantImp_NonAllocatorBase {
    // This class is component-private.  Do not use.  This class contains the
    // 'typedef's and data members of the 'Variant' class.  This class
    // serves as a base class for the variant when none of the types held by
    // the variant has the 'bslalg::TypeTraitUsesBslmaAllocator' type trait.
    // The goal is to optimize the size of the variant to avoid holding an
    // unnecessary allocator pointer.

  public:
    // TYPES
    typedef TYPES TypeList;
        // 'TypeList' is an alias for the 'bslmf::TypeList' type serving as the
        // template parameter to this variant implementation.

    typedef typename bslmf::TypeListTypeOf< 1, TYPES>::TypeOrDefault Type1;
    typedef typename bslmf::TypeListTypeOf< 2, TYPES>::TypeOrDefault Type2;
    typedef typename bslmf::TypeListTypeOf< 3, TYPES>::TypeOrDefault Type3;
    typedef typename bslmf::TypeListTypeOf< 4, TYPES>::TypeOrDefault Type4;
    typedef typename bslmf::TypeListTypeOf< 5, TYPES>::TypeOrDefault Type5;
    typedef typename bslmf::TypeListTypeOf< 6, TYPES>::TypeOrDefault Type6;
    typedef typename bslmf::TypeListTypeOf< 7, TYPES>::TypeOrDefault Type7;
    typedef typename bslmf::TypeListTypeOf< 8, TYPES>::TypeOrDefault Type8;
    typedef typename bslmf::TypeListTypeOf< 9, TYPES>::TypeOrDefault Type9;
    typedef typename bslmf::TypeListTypeOf<10, TYPES>::TypeOrDefault Type10;
    typedef typename bslmf::TypeListTypeOf<11, TYPES>::TypeOrDefault Type11;
    typedef typename bslmf::TypeListTypeOf<12, TYPES>::TypeOrDefault Type12;
    typedef typename bslmf::TypeListTypeOf<13, TYPES>::TypeOrDefault Type13;
    typedef typename bslmf::TypeListTypeOf<14, TYPES>::TypeOrDefault Type14;
    typedef typename bslmf::TypeListTypeOf<15, TYPES>::TypeOrDefault Type15;
    typedef typename bslmf::TypeListTypeOf<16, TYPES>::TypeOrDefault Type16;
    typedef typename bslmf::TypeListTypeOf<17, TYPES>::TypeOrDefault Type17;
    typedef typename bslmf::TypeListTypeOf<18, TYPES>::TypeOrDefault Type18;
    typedef typename bslmf::TypeListTypeOf<19, TYPES>::TypeOrDefault Type19;
    typedef typename bslmf::TypeListTypeOf<20, TYPES>::TypeOrDefault Type20;
        // 'TypeN' is an alias for the 'N'th type in the 'TypeList' of this
        // variant implementation.  If less than 'N' template arguments were
        // given to the 'Variant' type, then 'TypeN' is
        // 'bslmf::Nil'.

  private:
    union Value {
        bsls::ObjectBuffer<Type1>       d_v1;
        bsls::ObjectBuffer<Type2>       d_v2;
        bsls::ObjectBuffer<Type3>       d_v3;
        bsls::ObjectBuffer<Type4>       d_v4;
        bsls::ObjectBuffer<Type5>       d_v5;
        bsls::ObjectBuffer<Type6>       d_v6;
        bsls::ObjectBuffer<Type7>       d_v7;
        bsls::ObjectBuffer<Type8>       d_v8;
        bsls::ObjectBuffer<Type9>       d_v9;
        bsls::ObjectBuffer<Type10>      d_v10;
        bsls::ObjectBuffer<Type11>      d_v11;
        bsls::ObjectBuffer<Type12>      d_v12;
        bsls::ObjectBuffer<Type13>      d_v13;
        bsls::ObjectBuffer<Type14>      d_v14;
        bsls::ObjectBuffer<Type15>      d_v15;
        bsls::ObjectBuffer<Type16>      d_v16;
        bsls::ObjectBuffer<Type17>      d_v17;
        bsls::ObjectBuffer<Type18>      d_v18;
        bsls::ObjectBuffer<Type19>      d_v19;
        bsls::ObjectBuffer<Type20>      d_v20;
    };
        // 'Value' is a union of 'bsls::ObjectBuffer' of all types contained by
        // the variant.  'bsls::ObjectBuffer' is used to:  1) Wrap around non
        // "POD" types within the union.  2) Ensure proper alignment of the
        // type.

    // DATA
    Value            d_value;  // value of the object, initialized by child
                               // class

    int              d_type;   // current type the variant is holding

    // FRIENDS
    template <typename VARIANT_TYPES>
    friend class VariantImp;

    template <typename VARIANT_TYPES>
    friend bool operator==(const VariantImp<VARIANT_TYPES>&,
                           const VariantImp<VARIANT_TYPES>&);

  public:
    // CREATORS
    VariantImp_NonAllocatorBase(int type, bslma::Allocator *);

    VariantImp_NonAllocatorBase(int                type,
                                      bslma::Allocator  *,
                                      bslmf::MetaInt<1> *);
        // Create a 'VariantImp_AllocatorBase' with the specified 'type'
        // indicating the type the variant is currently holding.

    template <typename TYPE>
    VariantImp_NonAllocatorBase(int                type,
                                      const TYPE&,
                                      bslmf::MetaInt<0> *);

    // ACCESSORS
    bslma::Allocator *getAllocator() const;
        // Return a null pointer since this object does not hold an allocator
        // pointer.
};

                       // ==============================
                       // struct VariantImp_Traits
                       // ==============================

template <class TYPES>
struct VariantImp_Traits {
    // This struct is component-private.  Do not use.  Selects either
    // 'VariantImp_AllocatorBase' as a base class type if any one of
    // the twenty types held by the variant uses the
    // 'bslalg::TypeTraitUsesBslmaAllocator' trait, and
    // 'VariantImp_NonAllocatorBase' as the base class type otherwise.

    typedef typename bslmf::TypeListTypeOf< 1, TYPES>::TypeOrDefault Type1;
    typedef typename bslmf::TypeListTypeOf< 2, TYPES>::TypeOrDefault Type2;
    typedef typename bslmf::TypeListTypeOf< 3, TYPES>::TypeOrDefault Type3;
    typedef typename bslmf::TypeListTypeOf< 4, TYPES>::TypeOrDefault Type4;
    typedef typename bslmf::TypeListTypeOf< 5, TYPES>::TypeOrDefault Type5;
    typedef typename bslmf::TypeListTypeOf< 6, TYPES>::TypeOrDefault Type6;
    typedef typename bslmf::TypeListTypeOf< 7, TYPES>::TypeOrDefault Type7;
    typedef typename bslmf::TypeListTypeOf< 8, TYPES>::TypeOrDefault Type8;
    typedef typename bslmf::TypeListTypeOf< 9, TYPES>::TypeOrDefault Type9;
    typedef typename bslmf::TypeListTypeOf<10, TYPES>::TypeOrDefault Type10;
    typedef typename bslmf::TypeListTypeOf<11, TYPES>::TypeOrDefault Type11;
    typedef typename bslmf::TypeListTypeOf<12, TYPES>::TypeOrDefault Type12;
    typedef typename bslmf::TypeListTypeOf<13, TYPES>::TypeOrDefault Type13;
    typedef typename bslmf::TypeListTypeOf<14, TYPES>::TypeOrDefault Type14;
    typedef typename bslmf::TypeListTypeOf<15, TYPES>::TypeOrDefault Type15;
    typedef typename bslmf::TypeListTypeOf<16, TYPES>::TypeOrDefault Type16;
    typedef typename bslmf::TypeListTypeOf<17, TYPES>::TypeOrDefault Type17;
    typedef typename bslmf::TypeListTypeOf<18, TYPES>::TypeOrDefault Type18;
    typedef typename bslmf::TypeListTypeOf<19, TYPES>::TypeOrDefault Type19;
    typedef typename bslmf::TypeListTypeOf<20, TYPES>::TypeOrDefault Type20;

  public:
    enum {
//ARB:ENUM 1026
        VARIANT_USES_BSLMA_ALLOCATOR = (
            bslma::UsesBslmaAllocator< Type1>::value
         || bslma::UsesBslmaAllocator< Type2>::value
         || bslma::UsesBslmaAllocator< Type3>::value
         || bslma::UsesBslmaAllocator< Type4>::value
         || bslma::UsesBslmaAllocator< Type5>::value
         || bslma::UsesBslmaAllocator< Type6>::value
         || bslma::UsesBslmaAllocator< Type7>::value
         || bslma::UsesBslmaAllocator< Type8>::value
         || bslma::UsesBslmaAllocator< Type9>::value
         || bslma::UsesBslmaAllocator<Type10>::value
         || bslma::UsesBslmaAllocator<Type11>::value
         || bslma::UsesBslmaAllocator<Type12>::value
         || bslma::UsesBslmaAllocator<Type13>::value
         || bslma::UsesBslmaAllocator<Type14>::value
         || bslma::UsesBslmaAllocator<Type15>::value
         || bslma::UsesBslmaAllocator<Type16>::value
         || bslma::UsesBslmaAllocator<Type17>::value
         || bslma::UsesBslmaAllocator<Type18>::value
         || bslma::UsesBslmaAllocator<Type19>::value
         || bslma::UsesBslmaAllocator<Type20>::value),

//ARB:ENUM 1048
        VARIANT_IS_BITWISE_COPYABLE = (
            bsl::is_trivially_copyable< Type1>::value
         && bsl::is_trivially_copyable< Type2>::value
         && bsl::is_trivially_copyable< Type3>::value
         && bsl::is_trivially_copyable< Type4>::value
         && bsl::is_trivially_copyable< Type5>::value
         && bsl::is_trivially_copyable< Type6>::value
         && bsl::is_trivially_copyable< Type7>::value
         && bsl::is_trivially_copyable< Type8>::value
         && bsl::is_trivially_copyable< Type9>::value
         && bsl::is_trivially_copyable<Type10>::value
         && bsl::is_trivially_copyable<Type11>::value
         && bsl::is_trivially_copyable<Type12>::value
         && bsl::is_trivially_copyable<Type13>::value
         && bsl::is_trivially_copyable<Type14>::value
         && bsl::is_trivially_copyable<Type15>::value
         && bsl::is_trivially_copyable<Type16>::value
         && bsl::is_trivially_copyable<Type17>::value
         && bsl::is_trivially_copyable<Type18>::value
         && bsl::is_trivially_copyable<Type19>::value
         && bsl::is_trivially_copyable<Type20>::value),

//ARB:ENUM 1070
        VARIANT_IS_BITWISE_MOVEABLE = (
            bslmf::IsBitwiseMoveable< Type1>::value
         && bslmf::IsBitwiseMoveable< Type2>::value
         && bslmf::IsBitwiseMoveable< Type3>::value
         && bslmf::IsBitwiseMoveable< Type4>::value
         && bslmf::IsBitwiseMoveable< Type5>::value
         && bslmf::IsBitwiseMoveable< Type6>::value
         && bslmf::IsBitwiseMoveable< Type7>::value
         && bslmf::IsBitwiseMoveable< Type8>::value
         && bslmf::IsBitwiseMoveable< Type9>::value
         && bslmf::IsBitwiseMoveable<Type10>::value
         && bslmf::IsBitwiseMoveable<Type11>::value
         && bslmf::IsBitwiseMoveable<Type12>::value
         && bslmf::IsBitwiseMoveable<Type13>::value
         && bslmf::IsBitwiseMoveable<Type14>::value
         && bslmf::IsBitwiseMoveable<Type15>::value
         && bslmf::IsBitwiseMoveable<Type16>::value
         && bslmf::IsBitwiseMoveable<Type17>::value
         && bslmf::IsBitwiseMoveable<Type18>::value
         && bslmf::IsBitwiseMoveable<Type19>::value
         && bslmf::IsBitwiseMoveable<Type20>::value)
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
#endif  // BDE_OMIT_INTERNAL_DEPRECATED
    };

    typedef typename bslmf::If<
        VARIANT_USES_BSLMA_ALLOCATOR,
        VariantImp_AllocatorBase<TYPES>,
        VariantImp_NonAllocatorBase<TYPES> >::Type BaseType;
        // Determines what the base type is.
};

                       // =====================================
                       // class Variant_ReturnAnyTypeUtil
                       // =====================================

struct Variant_ReturnAnyTypeUtil {
    // This 'struct' provides a function that returns an (invalid) instance of
    // any type.  It is meant to allow clients to express:
    //..
    //  template <RESULT_TYPE>
    //  RESULT_TYPE foo() {
    //      // ...
    //
    //      // The following 'return' is unreachable, but is required for
    //      // compilation.
    //
    //      return Variant_ReturnAnyTypeUtil::garbage();
    //  }
    //..
    // where 'RESULT_TYPE' may be 'void'.  Note that while such a return
    // statement is not required by the C++ standard, the lack of such a
    // return statement will cause a warning (or error) on many compilers.

    // CLASS METHODS
    static void garbage(void *);
    template <class TYPE> static TYPE garbage(TYPE *dummy);
    template <class TYPE> static TYPE garbage();
        // Behavior is undefined if this method is called at runtime.  Note
        // that the overload for 'void *' is required because the template
        // function cannot be implemented generically when 'TYPE' is 'void'.
};


                       // ====================================
                       // class Variant_RawVisitorHelper
                       // ====================================

template <class RESULT_TYPE, class VISITOR>
class Variant_RawVisitorHelper {
    // This 'struct' provides a helper for implementing
    // 'Variant::applyRaw', which enables 'applyRaw' to support visitor
    // functors that do not provide an overload for 'operator()(bslmf::Nil)'.
    // Objects of this type are constructed using a functor of (template
    // parameter) type 'VISITOR', whose 'operator()' returns the (template
    // parameter) type 'RESULT_TYPE'.  A 'Variant_RawVisitorHelper' wraps a
    // functor of type 'VISITOR' and provides an implementation of
    // 'operator()(bslmf::Nil)' that performs a 'BSLS_ASSERT_OPT(false)'.  Note
    // that this overload is needed to enable compilation (specifically, to
    // instantiate 'doApply' and 'doApplyR'), but is never invoked by any code
    // path at runtime.

    // DATA
    VISITOR *d_visitor;  // visitor to which this helper delegates

  public:
    explicit Variant_RawVisitorHelper(VISITOR *visitor);
        // Create a 'RawVisitorHelper' functor that delegates to the specified
        // 'visitor'.

    RESULT_TYPE operator()(bslmf::Nil) const;
        // Assert 'false'.

    template <class ARGUMENT_TYPE>
    RESULT_TYPE operator()(ARGUMENT_TYPE& argument);
    template <class ARGUMENT_TYPE>
    RESULT_TYPE operator()(const ARGUMENT_TYPE& argument);
    template <class ARGUMENT_TYPE>
    RESULT_TYPE operator()(ARGUMENT_TYPE& argument) const;
    template <class ARGUMENT_TYPE>
    RESULT_TYPE operator()(const ARGUMENT_TYPE& argument) const;
        // Invoke the functor supplied at construction with the specified
        // 'argument' and return the result.
};

                       // =============================
                       // class VariantImp<TYPES>
                       // =============================

template <typename TYPES>
class VariantImp : public VariantImp_Traits<TYPES>::BaseType {
    // This class provides the implementation of 'Variant' (except for
    // the creators) given a parameterized list of 'TYPES'.
    //
    // More generally, if each of the types in the list of 'TYPES' is
    // value-semantic, then this class also supports a complete set of *value*
    // *semantic* operations, including copy construction, assignment, equality
    // comparison, 'ostream' printing, and 'bdex' serialization.  A precise
    // operational definition of when two instances have the same value can be
    // found in the description of 'operator==' for the class.  This class is
    // *exception* *neutral* with no guarantee of rollback: if an exception is
    // thrown during the invocation of a method on a pre-existing instance, the
    // object is left in a valid state, but its value is undefined.  In no
    // event is memory leaked.  Finally, *aliasing* (e.g., using all or part of
    // an object as both source and destination) is supported in all cases.
    //
    // If any of the types in the list of 'TYPES' does not support
    // 'operator==', or any of the value-semantic operations mentioned above,
    // then this variant also does not support that operation and attempts to
    // invoke it will trigger a compilation error.
    //
    // Finally, note the limitations on top-level 'bdex' serialization
    // mentioned in the "'bdex' Streamability" section of the component-level
    // documentation.

    // PRIVATE TYPES
    typedef VariantImp_Traits<TYPES>                           Traits;
    typedef typename Traits::BaseType                                Base;

  private:
    // PRIVATE MANIPULATORS
    template <class TYPE, class VISITOR_REF>
    void applyImp(VISITOR_REF visitor, bslmf::MetaInt<0> *);
    template <class TYPE, class VISITOR_REF>
    void applyImp(VISITOR_REF visitor, bslmf::MetaInt<1> *);
    template <class TYPE, class VISITOR_REF>
    void applyImp(VISITOR_REF visitor);
        // Invoke 'operator()' of the specified 'visitor' on the current
        // value (of parameterized 'TYPE') held by this variant.  Note that the
        // second argument is for resolving overloading ambiguity and is not
        // used.

    template <class TYPE, class VISITOR_REF, class RET_TYPE>
    RET_TYPE applyImpR(VISITOR_REF visitor, bslmf::MetaInt<0> *);
    template <class TYPE, class VISITOR_REF, class RET_TYPE>
    RET_TYPE applyImpR(VISITOR_REF visitor, bslmf::MetaInt<1> *);
    template <class TYPE, class VISITOR_REF, class RET_TYPE>
    RET_TYPE applyImpR(VISITOR_REF visitor);
        // Invoke 'operator()' of the specified 'visitor' on the current
        // value (of parameterized 'TYPE') held by this variant and return the
        // specified 'RET_TYPE'.  Note that the second argument is for
        // resolving overloading ambiguity and is not used.

    template <class TYPE, class SOURCE_TYPE_REF>
    void assignImp(const SOURCE_TYPE_REF& value);
        // Assign the specified 'value' of parameterized 'SOURCE_TYPE_REF' to
        // this variant.

    template <class TYPE>
    void create(const TYPE& value, bslmf::MetaInt<0> *);
        // Construct this variant object with the specified 'value'.  The
        // second parameter is for resolving overloading ambiguity and is not
        // used.

    void create(bslma::Allocator *, bslmf::MetaInt<1> *);
        // Construct this variant object in its unset state.

    template <class VISITOR_REF>
    void doApply(VISITOR_REF visitor, int type);
        // Apply the specified 'visitor' on the current value held by this
        // variant by invoking 'applyImp' with the appropriate template
        // arguments, determined by the specified 'type'.  The behavior is
        // undefined if type is 0.

    template <class VISITOR_REF, class RET_TYPE>
    RET_TYPE doApplyR(VISITOR_REF visitor, int type);
        // Apply the specified 'visitor' on the current value held by this
        // variant by invoking 'applyImpR' with the appropriate template
        // arguments, determined by the specified 'type'.  Return the specified
        // 'RET_TYPE'.  The behavior is undefined if type is 0.

    // PRIVATE ACCESSORS
    template <class TYPE, class VISITOR_REF>
    void applyImp(VISITOR_REF visitor, bslmf::MetaInt<0> *) const;
    template <class TYPE, class VISITOR_REF>
    void applyImp(VISITOR_REF visitor, bslmf::MetaInt<1> *) const;
    template <class TYPE, class VISITOR_REF>
    void applyImp(VISITOR_REF visitor) const;
        // Invoke 'operator()' of the specified 'visitor' on the current
        // value (of parameterized 'TYPE') held by this variant.  Note that the
        // second argument is for resolving overloading ambiguity and is not
        // used.

    template <class TYPE, class VISITOR_REF, class RET_TYPE>
    RET_TYPE applyImpR(VISITOR_REF visitor, bslmf::MetaInt<0> *) const;
    template <class TYPE, class VISITOR_REF, class RET_TYPE>
    RET_TYPE applyImpR(VISITOR_REF visitor, bslmf::MetaInt<1> *) const;
    template <class TYPE, class VISITOR_REF, class RET_TYPE>
    RET_TYPE applyImpR(VISITOR_REF visitor) const;
        // Invoke 'operator()' of the specified 'visitor' on the current
        // value (of parameterized 'TYPE') held by this variant and return the
        // specified 'RET_TYPE'.  Note that the second argument is for
        // resolving overloading ambiguity and is not used.

    template <class VISITOR_REF>
    void doApply(VISITOR_REF visitor, int type) const;
        // Apply the specified 'visitor' on the current value held by this
        // variant by invoking 'applyImp' with the appropriate template
        // arguments, determined by the specified 'type'.  The behavior is
        // undefined if type is 0.

    template <class VISITOR_REF, class RET_TYPE>
    RET_TYPE doApplyR(VISITOR_REF visitor, int type) const;
        // Apply the specified 'visitor' on the current value held by this
        // variant by invoking 'applyImpR' with the appropriate template
        // arguments, determined by the specified 'type'.  Return the specified
        // 'RET_TYPE'.  The behavior is undefined if type is 0.

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION_IF(VariantImp,
                                      bslma::UsesBslmaAllocator,
                                      Traits::VARIANT_USES_BSLMA_ALLOCATOR);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(VariantImp,
                                      bsl::is_trivially_copyable,
                                      Traits::VARIANT_IS_BITWISE_COPYABLE);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(VariantImp,
                                      bslmf::IsBitwiseMoveable,
                                      Traits::VARIANT_IS_BITWISE_MOVEABLE);
    BSLMF_NESTED_TRAIT_DECLARATION(VariantImp, bdlb::HasPrintMethod);

    // CREATORS
    VariantImp();
        // Create a variant object.  The type and value of this object are
        // unset.  Use the currently installed default allocator to supply
        // memory.

    template <class TYPE_OR_ALLOCATOR>
    explicit VariantImp(const TYPE_OR_ALLOCATOR& typeOrAlloc);
        // Create a variant object with the specified 'typeOrAlloc', which can
        // either be a type that the variant can hold, or an allocator to
        // supply memory.  If 'typeOrAlloc' is not a 'bslma::Allocator *', then
        // the variant will hold the value and type of 'typeOrAlloc', using
        // the currently installed default allocator to supply memory.
        // Otherwise, the variant will default construct to an unset type and
        // value using 'typeOrAlloc' to supply memory.  Note that this
        // templated constructor is created instead of having two constructors
        // (one takes a 'bslma::Allocator *' and the other templated) is
        // because templated arguments are always a better match than
        // child-to-base conversion (concrete allocator implementation pointer
        // converted to 'bslma::Allocator *').

    template <class TYPE>
    VariantImp(const TYPE& value, bslma::Allocator *basicAllocator);
        // Create a variant object having the specified 'value' and that uses
        // the specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    VariantImp(const VariantImp&  original,
                     bslma::Allocator        *basicAllocator = 0);
        // Create a variant implementation having the type and value of the
        // specified 'original' variant implementation.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    ~VariantImp();
        // Destroy this variant object, invoking the destructor of the type of
        // object contained (if any) on the value of that type.

    // MANIPULATORS
    template <class TYPE>
    VariantImp& operator=(const TYPE& value);
        // Assign to this object the specified 'value' of parameterized 'TYPE',
        // and return a reference to this modifiable object.  The value
        // previously held by this variant (if any) will be destroyed if the
        // value's type is different from the parameterized 'TYPE'.

    VariantImp& operator=(const VariantImp& rhs);
        // Assign to this object the type and value currently hold by the
        // specified 'rhs' object, and return a reference to this modifiable
        // object.  The value previously held by this variant (if any) will be
        // destroyed if the value's type is different from the type held by the
        // 'rhs' object.

    template <class VISITOR>
    typename bsl::enable_if<
                          Variant_ReturnValueHelper<VISITOR>::k_VaL == 1,
                          typename VISITOR::ResultType>::type
    apply(VISITOR& visitor) {
        // Apply the specified 'visitor' to this modifiable variant by passing
        // the value this variant currently holds to 'visitor' object's
        // 'operator()', and return the value returned by the 'visitor'
        // object's 'operator()'.  If the variant is unset, a default
        // constructed 'bslmf::Nil' will be passed to the visitor.  This method
        // is selected only if the templatized type 'VISITOR' defines a
        // 'typedef' of 'ResultType' in its public interface.  Note that this
        // method is declared in place inline to workaround a windows compiler
        // bug with SFINAE functions.

        if (this->d_type) {
            return doApplyR<VISITOR&,
                          typename VISITOR::ResultType>(visitor, this->d_type);
        }

        bslmf::Nil nil = bslmf::Nil();
        return visitor(nil);
    }

    template <class VISITOR>
    typename bsl::enable_if<
                          Variant_ReturnValueHelper<VISITOR>::k_VaL == 1,
                          typename VISITOR::ResultType>::type
    apply(const VISITOR& visitor) {
        // Apply the specified 'visitor' to this modifiable variant by passing
        // the value this variant currently holds to 'visitor' object's
        // 'operator()', and return the value returned by the 'visitor'
        // object's 'operator()'.  If the variant is unset, a default
        // constructed 'bslmf::Nil' will be passed to the visitor.  This method
        // is selected only if the templatized type 'VISITOR' defines a
        // 'typedef' of 'ResultType' in its public interface.  Note that this
        // method is declared in place inline to workaround a windows compiler
        // bug with SFINAE functions.

        if (this->d_type) {
            return doApplyR<const VISITOR&,
                          typename VISITOR::ResultType>(visitor, this->d_type);
        }

        bslmf::Nil nil = bslmf::Nil();
        return visitor(nil);
    }

    template <class VISITOR, class TYPE>
    typename bsl::enable_if<
                          Variant_ReturnValueHelper<VISITOR>::k_VaL == 1,
                          typename VISITOR::ResultType>::type
    apply(VISITOR& visitor, const TYPE& defaultValue) {
        // Apply the specified 'visitor' to this modifiable variant by passing
        // the value this variant currently holds to 'visitor' object's
        // 'operator()', and return the value returned by the 'visitor'
        // object's 'operator()'.  If the variant is unset, the specified
        // 'defaultValue' will be passed to the visitor.  This method is
        // selected only if the templatized type 'VISITOR' defines a 'typedef'
        // of 'ResultType' in its public interface.  Note that this method is
        // declared in place inline to workaround a windows compiler bug with
        // SFINAE functions.

        if (this->d_type) {
            return doApplyR<VISITOR&,
                          typename VISITOR::ResultType>(visitor, this->d_type);
        }

        return visitor(defaultValue);
    }

    template <class VISITOR, class TYPE>
    typename bsl::enable_if<
                          Variant_ReturnValueHelper<VISITOR>::k_VaL == 1,
                          typename VISITOR::ResultType>::type
    apply(const VISITOR& visitor, const TYPE& defaultValue) {
        // Apply the specified 'visitor' to this modifiable variant by passing
        // the value this variant currently holds to 'visitor' object's
        // 'operator()', and return the value returned by the 'visitor'
        // object's 'operator()'.  If the variant is unset, the specified
        // 'defaultValue' will be passed to the visitor.  This method is
        // selected only if the templatized type 'VISITOR' defines a 'typedef'
        // of 'ResultType' in its public interface.  Note that this method is
        // declared in place inline to workaround a windows compiler bug with
        // SFINAE functions.

        if (this->d_type) {
            return doApplyR<const VISITOR&,
                          typename VISITOR::ResultType>(visitor, this->d_type);
        }

        return visitor(defaultValue);
    }

    template <class VISITOR>
    typename bsl::enable_if<
             Variant_ReturnValueHelper<VISITOR>::k_VaL == 0, void>::type
    apply(VISITOR& visitor) {
        // Apply the specified 'visitor' to this modifiable variant by passing
        // the value this variant currently holds to 'visitor' object's
        // 'operator()'.  This method does not return any values.  If the
        // variant is unset, a default constructed 'bslmf::Nil' will be passed
        // to the visitor.  This method is selected only if the templatized
        // type 'VISITOR' does not define a 'typedef' of 'ResultType' in its
        // public interface.  Note that this method is declared in place inline
        // to workaround a windows compiler bug with SFINAE functions.

        if (this->d_type) {
            doApply<VISITOR&>(visitor, this->d_type);
            return;
        }

        bslmf::Nil nil = bslmf::Nil();
        visitor(nil);
    }

    template <class VISITOR>
    typename bsl::enable_if<
             Variant_ReturnValueHelper<VISITOR>::k_VaL == 0, void>::type
    apply(const VISITOR& visitor) {
        // Apply the specified 'visitor' to this modifiable variant by passing
        // the value this variant currently holds to 'visitor' object's
        // 'operator()'.  This method does not return any values.  If the
        // variant is unset, a default constructed 'bslmf::Nil' will be passed
        // to the visitor.  This method is selected only if the templatized
        // type 'VISITOR' does not define a 'typedef' of 'ResultType' in its
        // public interface.  Note that this method is declared in place inline
        // to workaround a windows compiler bug with SFINAE functions.

        if (this->d_type) {
            doApply<const VISITOR&>(visitor, this->d_type);
            return;
        }

        bslmf::Nil nil = bslmf::Nil();
        visitor(nil);
    }

    template <class VISITOR, class TYPE>
    typename bsl::enable_if<
             Variant_ReturnValueHelper<VISITOR>::k_VaL == 0, void>::type
    apply(VISITOR& visitor, const TYPE& defaultValue) {
        // Apply the specified 'visitor' to this modifiable variant by passing
        // the value this variant currently holds to 'visitor' object's
        // 'operator()'.  This method does not return any values.  If the
        // variant is unset, the specified 'defaultValue' will be passed to the
        // visitor.  This method is selected only if the templatized type
        // 'VISITOR' does not define a 'typedef' of 'ResultType' in its public
        // interface.  Note that this method is declared in place inline to
        // workaround a windows compiler bug with SFINAE functions.

        if (this->d_type) {
            doApply<VISITOR&>(visitor, this->d_type);
            return;
        }

        visitor(defaultValue);
    }

    template <class VISITOR, class TYPE>
    typename bsl::enable_if<
             Variant_ReturnValueHelper<VISITOR>::k_VaL == 0, void>::type
    apply(const VISITOR& visitor, const TYPE& defaultValue) {
        // Apply the specified 'visitor' to this modifiable variant by passing
        // the value this variant currently holds to 'visitor' object's
        // 'operator()'.  This method does not return any values.  If the
        // variant is unset, the specified 'defaultValue' will be passed to the
        // visitor.  This method is selected only if the templatized type
        // 'VISITOR' does not define a 'typedef' of 'ResultType' in its public
        // interface.  Note that this method is declared in place inline to
        // workaround a windows compiler bug with SFINAE functions.

        if (this->d_type) {
            doApply<const VISITOR&>(visitor, this->d_type);
            return;
        }

        visitor(defaultValue);
    }

    template <class RET_TYPE, class VISITOR>
    RET_TYPE apply(VISITOR& visitor);
    template <class RET_TYPE, class VISITOR>
    RET_TYPE apply(const VISITOR& visitor);
        // Apply the specified 'visitor' to this modifiable variant by passing
        // the value this variant currently holds to 'visitor' object's
        // 'operator()', and return the value returned by the 'visitor'
        // object's 'operator()' of templatized 'RET_TYPE'.  If the variant is
        // unset, a default constructed 'bslmf::Nil' will be passed to the
        // visitor.

    template <class RET_TYPE, class VISITOR, class TYPE>
    RET_TYPE apply(VISITOR& visitor, const TYPE& defaultValue);
    template <class RET_TYPE, class VISITOR, class TYPE>
    RET_TYPE apply(const VISITOR& visitor, const TYPE& defaultValue);
        // Apply the specified 'visitor' to this modifiable variant by passing
        // the value this variant currently holds to 'visitor' object's
        // 'operator()', and return the value returned by the 'visitor'
        // object's 'operator()' of templatized 'RET_TYPE'.  If the variant is
        // unset, the specified 'defaultValue' will be passed to the visitor.

    template <class VISITOR>
    typename bsl::enable_if<
                          Variant_ReturnValueHelper<VISITOR>::k_VaL == 1,
                          typename VISITOR::ResultType>::type
    applyRaw(VISITOR& visitor) {
        // Apply the specified 'visitor' to this modifiable variant by passing
        // the value this variant currently holds to 'visitor' object's
        // 'operator()', and return the value returned by the 'visitor'
        // object's 'operator()'.  This method is selected only if the
        // templatized type 'VISITOR' defines a 'typedef' of 'ResultType' in
        // its public interface.  The behavior is undefined if the variant is
        // unset.  Note that this method is declared in place inline to
        // workaround a windows compiler bug with SFINAE functions.

        typedef Variant_RawVisitorHelper<
                               typename VISITOR::ResultType,
                               VISITOR> Helper;

        return doApplyR<const Helper&,
                       typename VISITOR::ResultType>(Helper(&visitor),
                                                     this->d_type);
    }

    template <class VISITOR>
    typename bsl::enable_if<
                          Variant_ReturnValueHelper<VISITOR>::k_VaL == 1,
                          typename VISITOR::ResultType>::type
    applyRaw(const VISITOR& visitor) {
        // Apply the specified 'visitor' to this modifiable variant by passing
        // the value this variant currently holds to 'visitor' object's
        // 'operator()', and return the value returned by the 'visitor'
        // object's 'operator()'.  This method is selected only if the
        // templatized type 'VISITOR' defines a 'typedef' of 'ResultType' in
        // its public interface.  The behavior is undefined if the variant is
        // unset.  Note that this method is declared in place inline to
        // workaround a windows compiler bug with SFINAE functions.

        typedef Variant_RawVisitorHelper<
                               typename VISITOR::ResultType,
                               const VISITOR> Helper;

        return doApplyR<const Helper&,
                       typename VISITOR::ResultType>(Helper(&visitor),
                                                     this->d_type);
    }

    template <class VISITOR>
    typename bsl::enable_if<
             Variant_ReturnValueHelper<VISITOR>::k_VaL == 0, void>::type
    applyRaw(VISITOR& visitor) {
        // Apply the specified 'visitor' to this modifiable variant by passing
        // the value this variant currently holds to 'visitor' object's
        // 'operator()'.  This method does not return any values.  This method
        // is selected only if the templatized type 'VISITOR' does not define a
        // 'typedef' of 'ResultType' in its public interface.  The behavior is
        // undefined if the variant is unset.  Note that this method is
        // declared in place inline to workaround a windows compiler bug with
        // SFINAE functions.


        typedef Variant_RawVisitorHelper<void, VISITOR> Helper;

        doApply<const Helper&>(Helper(&visitor), this->d_type);

    }

    template <class VISITOR>
    typename bsl::enable_if<
             Variant_ReturnValueHelper<VISITOR>::k_VaL == 0, void>::type
    applyRaw(const VISITOR& visitor) {
        // Apply the specified 'visitor' to this modifiable variant by passing
        // the value this variant currently holds to 'visitor' object's
        // 'operator()'.  This method does not return any values.  This method
        // is selected only if the templatized type 'VISITOR' does not define a
        // 'typedef' of 'ResultType' in its public interface.  The behavior is
        // undefined if the variant is unset.  Note that this method is
        // declared in place inline to workaround a windows compiler bug with
        // SFINAE functions.

        typedef Variant_RawVisitorHelper<void, const VISITOR> Helper;

        doApply<const Helper&>(Helper(&visitor), this->d_type);
    }

    template <class RET_TYPE, class VISITOR>
    RET_TYPE applyRaw(VISITOR& visitor);
    template <class RET_TYPE, class VISITOR>
    RET_TYPE applyRaw(const VISITOR& visitor);
        // Apply the specified 'visitor' to this modifiable variant by passing
        // the value this variant currently holds to 'visitor' object's
        // 'operator()', and return the value returned by the 'visitor'
        // object's 'operator()' of templatized 'RET_TYPE'.  The behavior is
        // undefined if the current state of the variant is unset.

    template <class TYPE>
    VariantImp& assign(const TYPE& value);
        // Assign to this object the specified 'value' of the specified 'TYPE',
        // and return a reference to this modifiable object.  The value
        // previously held by this variant (if any) will be destroyed if the
        // value's type is different from the parameterized 'TYPE'.

    template <class TYPE, class SOURCE_TYPE>
    VariantImp& assignTo(const SOURCE_TYPE& value);
        // Assign to this object the specified 'value' of the parameterized
        // 'SOURCE_TYPE' and return a reference to this modifiable object.
        // The value previously held by this variant (if any) will be destroyed
        // if the value's type is different from the parameterized 'TYPE'.

        // Note the order of the template arguments, chosen so that 'TYPE' must
        // always be specified.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format and return a reference
        // to the modifiable 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation, this object is valid, but its value is undefined.  If
        // 'version' is not supported, 'stream' is marked invalid and this
        // object is unaltered.  Note that no version is read from 'stream'.
        // See the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.

    template <class TYPE>
    void createInPlace();
    template <class TYPE, class A1>
    void createInPlace(const A1& a1);
    template <class TYPE, class A1, class A2>
    void createInPlace(const A1& a1, const A2& a2);
    template <class TYPE, class A1, class A2, class A3>
    void createInPlace(const A1& a1, const A2& a2, const A3& a3);
    template <class TYPE, class A1, class A2, class A3, class A4>
    void createInPlace(const A1& a1, const A2& a2, const A3& a3, const A4& a4);
    template <class TYPE, class A1, class A2, class A3, class A4, class A5>
    void createInPlace(const A1& a1, const A2& a2, const A3& a3, const A4& a4,
                       const A5& a5);
    template <class TYPE, class A1, class A2, class A3, class A4, class A5,
                          class A6>
    void createInPlace(const A1& a1, const A2& a2, const A3& a3, const A4& a4,
                       const A5& a5, const A6& a6);
    template <class TYPE, class A1, class A2, class A3, class A4, class A5,
                          class A6, class A7>
    void createInPlace(const A1& a1, const A2& a2, const A3& a3, const A4& a4,
                       const A5& a5, const A6& a6, const A7& a7);
    template <class TYPE, class A1, class A2, class A3, class A4, class A5,
                          class A6, class A7, class A8>
    void createInPlace(const A1& a1, const A2& a2, const A3& a3, const A4& a4,
                       const A5& a5, const A6& a6, const A7& a7, const A8& a8);
    template <class TYPE, class A1, class A2, class A3, class A4, class A5,
                          class A6, class A7, class A8, class A9>
    void createInPlace(const A1& a1, const A2& a2, const A3& a3, const A4& a4,
                       const A5& a5, const A6& a6, const A7& a7, const A8& a8,
                       const A9& a9);
    template <class TYPE, class A1, class A2, class A3, class A4, class A5,
                          class A6, class A7, class A8, class A9, class A10>
    void createInPlace(const A1& a1, const A2& a2, const A3& a3, const A4& a4,
                       const A5& a5, const A6& a6, const A7& a7, const A8& a8,
                       const A9& a9, const A10& a10);
    template <class TYPE, class A1, class A2, class A3, class A4, class A5,
                          class A6, class A7, class A8, class A9, class A10,
                          class A11>
    void createInPlace(const A1& a1, const A2& a2, const A3& a3, const A4& a4,
                       const A5& a5, const A6& a6, const A7& a7, const A8& a8,
                       const A9& a9, const A10& a10, const A11& a11);
    template <class TYPE, class A1, class A2, class A3, class A4, class A5,
                          class A6, class A7, class A8, class A9, class A10,
                          class A11, class A12>
    void createInPlace(const A1& a1, const A2& a2, const A3& a3, const A4& a4,
                       const A5& a5, const A6& a6, const A7& a7, const A8& a8,
                       const A9& a9, const A10& a10, const A11& a11,
                       const A12& a12);
    template <class TYPE, class A1, class A2, class A3, class A4, class A5,
                          class A6, class A7, class A8, class A9, class A10,
                          class A11, class A12, class A13>
    void createInPlace(const A1& a1, const A2& a2, const A3& a3, const A4& a4,
                       const A5& a5, const A6& a6, const A7& a7, const A8& a8,
                       const A9& a9, const A10& a10, const A11& a11,
                       const A12& a12, const A13& a13);
    template <class TYPE, class A1, class A2, class A3, class A4, class A5,
                          class A6, class A7, class A8, class A9, class A10,
                          class A11, class A12, class A13, class A14>
    void createInPlace(const A1& a1, const A2& a2, const A3& a3, const A4& a4,
                       const A5& a5, const A6& a6, const A7& a7, const A8& a8,
                       const A9& a9, const A10& a10, const A11& a11,
                       const A12& a12, const A13& a13, const A14& a14);
        // Construct an instance of the parameterized 'TYPE' in this variant
        // object with up to 14 parameters using the allocator currently held
        // by this variant.  This method first destroys the current type held
        // by the variant (even if 'TYPE' is the same as the current type
        // held).  The behavior is undefined unless 'TYPE' is one of the types
        // that this variant holds.

    void reset();
        // Destroy the current type held by this variant, and assign to this
        // variant the default unset value.

    void swap(VariantImp& other);
        // Swap the value of this object with the value of the specified
        // 'other' object.  This method provides the no-throw guarantee if the
        // 'TYPE' template parameter has a no-throw 'swap' and the two variant
        // objects being swapped have the same type.  Otherwise it provides the
        // basic guarantee.

    template <class TYPE>
    TYPE& the();
        // Return a reference to the modifiable value of the parameterized
        // 'TYPE' held by this variant object.  The behavior is undefined
        // unless 'is<TYPE>()' returns 'true' and 'TYPE' is not 'void'.  Note
        // that 'TYPE' must be specified explicitly, e.g.,
        // 'myValue.the<int>()'.

    // ACCESSORS
    template <class VISITOR>
    typename bsl::enable_if<
                          Variant_ReturnValueHelper<VISITOR>::k_VaL == 1,
                          typename VISITOR::ResultType>::type
    apply(VISITOR& visitor) const {
        // Apply the specified 'visitor' to this variant by passing the value
        // this variant currently holds to 'visitor' object's 'operator()', and
        // return the value returned by the 'visitor' object's 'operator()'.
        // If the variant is unset, a default constructed 'bslmf::Nil' will be
        // passed to the visitor.  This method is selected only if the
        // templatized type 'VISITOR' defines a 'typedef' of 'ResultType' in
        // its public interface.  Note that this method is declared in place
        // inline to workaround a windows compiler bug with SFINAE functions.

        if (this->d_type) {
            return doApplyR<VISITOR&,
                          typename VISITOR::ResultType>(visitor, this->d_type);
        }

        bslmf::Nil nil = bslmf::Nil();
        return visitor(nil);
    }

    template <class VISITOR>
    typename bsl::enable_if<
                          Variant_ReturnValueHelper<VISITOR>::k_VaL == 1,
                          typename VISITOR::ResultType>::type
    apply(const VISITOR& visitor) const {
        // Apply the specified 'visitor' to this variant by passing the value
        // this variant currently holds to 'visitor' object's 'operator()', and
        // return the value returned by the 'visitor' object's 'operator()'.
        // If the variant is unset, a default constructed 'bslmf::Nil' will be
        // passed to the visitor.  This method is selected only if the
        // templatized type 'VISITOR' defines a 'typedef' of 'ResultType' in
        // its public interface.  Note that this method is declared in place
        // inline to workaround a windows compiler bug with SFINAE functions.

        if (this->d_type) {
            return doApplyR<const VISITOR&,
                          typename VISITOR::ResultType>(visitor, this->d_type);
        }

        bslmf::Nil nil = bslmf::Nil();
        return visitor(nil);
    }

    template <class VISITOR, class TYPE>
    typename bsl::enable_if<
                          Variant_ReturnValueHelper<VISITOR>::k_VaL == 1,
                          typename VISITOR::ResultType>::type
    apply(VISITOR& visitor, const TYPE& defaultValue) const {
        // Apply the specified 'visitor' to this variant by passing the value
        // this variant currently holds to 'visitor' object's 'operator()', and
        // return the value returned by the 'visitor' object's 'operator()'.
        // If the variant is unset, the specified 'defaultValue' will be passed
        // to the visitor.  This method is selected only if the templatized
        // type 'VISITOR' defines a 'typedef' of 'ResultType' in its public
        // interface.  Note that this method is declared in place inline to
        // workaround a windows compiler bug with SFINAE functions.

        if (this->d_type) {
            return doApplyR<VISITOR&,
                          typename VISITOR::ResultType>(visitor, this->d_type);
        }

        return visitor(defaultValue);
    }

    template <class VISITOR, class TYPE>
    typename bsl::enable_if<
                          Variant_ReturnValueHelper<VISITOR>::k_VaL == 1,
                          typename VISITOR::ResultType>::type
    apply(const VISITOR& visitor, const TYPE& defaultValue) const {
        // Apply the specified 'visitor' to this variant by passing the value
        // this variant currently holds to 'visitor' object's 'operator()', and
        // return the value returned by the 'visitor' object's 'operator()'.
        // If the variant is unset, the specified 'defaultValue' will be passed
        // to the visitor.  This method is selected only if the templatized
        // type 'VISITOR' defines a 'typedef' of 'ResultType' in its public
        // interface.  Note that this method is declared in place inline to
        // workaround a windows compiler bug with SFINAE functions.

        if (this->d_type) {
            return doApplyR<const VISITOR&,
                          typename VISITOR::ResultType>(visitor, this->d_type);
        }

        return visitor(defaultValue);
    }

    template <class VISITOR>
    typename bsl::enable_if<
             Variant_ReturnValueHelper<VISITOR>::k_VaL == 0, void>::type
    apply(VISITOR& visitor) const {
        // Apply the specified 'visitor' to this variant by passing the value
        // this variant currently holds to 'visitor' object's 'operator()'.
        // This method does not return any values.  If the variant is unset, a
        // default constructed 'bslmf::Nil' will be passed to the visitor.
        // This method is selected only if the templatized type 'VISITOR' does
        // not define a 'typedef' of 'ResultType' in its public interface.
        // Note that this method is declared in place inline to workaround a
        // Windows compiler bug with SFINAE functions.

        if (this->d_type) {
            doApply<VISITOR&>(visitor, this->d_type);
            return;
        }

        bslmf::Nil nil = bslmf::Nil();
        visitor(nil);
    }

    template <class VISITOR>
    typename bsl::enable_if<
             Variant_ReturnValueHelper<VISITOR>::k_VaL == 0, void>::type
    apply(const VISITOR& visitor) const {
        // Apply the specified 'visitor' to this variant by passing the value
        // this variant currently holds to 'visitor' object's 'operator()'.
        // This method does not return any values.  If the variant is unset, a
        // default constructed 'bslmf::Nil' will be passed to the visitor.
        // This method is selected only if the templatized type 'VISITOR' does
        // not define a 'typedef' of 'ResultType' in its public interface.
        // Note that this method is declared in place inline to workaround a
        // Windows  compiler bug with SFINAE functions.

        if (this->d_type) {
            doApply<const VISITOR&>(visitor, this->d_type);
            return;
        }

        bslmf::Nil nil = bslmf::Nil();
        visitor(nil);
    }

    template <class VISITOR, class TYPE>
    typename bsl::enable_if<
             Variant_ReturnValueHelper<VISITOR>::k_VaL == 0, void>::type
    apply(VISITOR& visitor, const TYPE& defaultValue) const {
        // Apply the specified 'visitor' to this variant by passing the value
        // this variant currently holds to 'visitor' object's 'operator()'.
        // This method does not return any values.  If the variant is unset,
        // the specified 'defaultValue' will be passed to the visitor.  This
        // method is selected only if the templatized type 'VISITOR' does not
        // define a 'typedef' of 'ResultType' in its public interface.  Note
        // that this method is declared in place inline to workaround a windows
        // compiler bug with SFINAE functions.

        if (this->d_type) {
            doApply<VISITOR&>(visitor, this->d_type);
            return;
        }

        visitor(defaultValue);
    }

    template <class VISITOR, class TYPE>
    typename bsl::enable_if<
             Variant_ReturnValueHelper<VISITOR>::k_VaL == 0, void>::type
    apply(const VISITOR& visitor, const TYPE& defaultValue) const {
        // Apply the specified 'visitor' to this variant by passing the value
        // this variant currently holds to 'visitor' object's 'operator()'.
        // This method does not return any values.  If the variant is unset,
        // the specified 'defaultValue' will be passed to the visitor.  This
        // method is selected only if the templatized type 'VISITOR' does not
        // define a 'typedef' of 'ResultType' in its public interface.  Note
        // that this method is declared in place inline to workaround a windows
        // compiler bug with SFINAE functions.

        if (this->d_type) {
            doApply<const VISITOR&>(visitor, this->d_type);
            return;
        }

        visitor(defaultValue);
    }

    template <class RET_TYPE, class VISITOR>
    RET_TYPE apply(VISITOR& visitor) const;
    template <class RET_TYPE, class VISITOR>
    RET_TYPE apply(const VISITOR& visitor) const;
        // Apply the specified 'visitor' to this variant by passing the value
        // this variant currently holds to 'visitor' object's 'operator()', and
        // return the value returned by the 'visitor' object's 'operator()' of
        // templatized 'RET_TYPE'.  If the variant is unset, a default
        // constructed 'bslmf::Nil' will be passed to the visitor.

    template <class RET_TYPE, class VISITOR, class TYPE>
    RET_TYPE apply(VISITOR& visitor, const TYPE& defaultValue) const;
    template <class RET_TYPE, class VISITOR, class TYPE>
    RET_TYPE apply(const VISITOR& visitor, const TYPE& defaultValue) const;
        // Apply the specified 'visitor' to this variant by passing the value
        // this variant currently holds to 'visitor' object's 'operator()', and
        // return the value returned by the 'visitor' object's 'operator()' of
        // templatized 'RET_TYPE'.  If the variant is unset, the specified
        // 'defaultValue' will be passed to the visitor.

    template <class VISITOR>
    typename bsl::enable_if<
                          Variant_ReturnValueHelper<VISITOR>::k_VaL == 1,
                          typename VISITOR::ResultType>::type
    applyRaw(VISITOR& visitor) const {
        // Apply the specified 'visitor' to this variant by passing the value
        // this variant currently holds to 'visitor' object's 'operator()', and
        // return the value returned by the 'visitor' object's 'operator()'.
        // This method is selected only if the templatized type 'VISITOR'
        // defines a 'typedef' of 'ResultType' in its public interface.  The
        // behavior is undefined if the variant is unset.  Note that this
        // method is declared in place inline to workaround a windows compiler
        // bug with SFINAE functions.

        typedef Variant_RawVisitorHelper<
                               typename VISITOR::ResultType,
                               VISITOR> Helper;


        return doApplyR<const Helper&,
                       typename VISITOR::ResultType>(Helper(&visitor),
                                                     this->d_type);

    }

    template <class VISITOR>
    typename bsl::enable_if<
                          Variant_ReturnValueHelper<VISITOR>::k_VaL == 1,
                          typename VISITOR::ResultType>::type
    applyRaw(const VISITOR& visitor) const {
        // Apply the specified 'visitor' to this variant by passing the value
        // this variant currently holds to 'visitor' object's 'operator()', and
        // return the value returned by the 'visitor' object's 'operator()'.
        // This method is selected only if the templatized type 'VISITOR'
        // defines a 'typedef' of 'ResultType' in its public interface.  The
        // behavior is undefined if the variant is unset.  Note that this
        // method is declared in place inline to workaround a windows compiler
        // bug with SFINAE functions.

        typedef Variant_RawVisitorHelper<
                               typename VISITOR::ResultType,
                               const VISITOR> Helper;


        return doApplyR<const Helper&,
                 typename VISITOR::ResultType>(Helper(&visitor),
                                               this->d_type);
    }

    template <class VISITOR>
    typename bsl::enable_if<
             Variant_ReturnValueHelper<VISITOR>::k_VaL == 0, void>::type
    applyRaw(VISITOR& visitor) const {
        // Apply the specified 'visitor' to this variant by passing the value
        // this variant currently holds to 'visitor' object's 'operator()'.
        // This method does not return any values.  This method is selected
        // only if the templatized type 'VISITOR' does not define a 'typedef'
        // of 'ResultType' in its public interface.  The behavior is undefined
        // if the variant is unset.  Note that this method is declared in place
        // inline to workaround a windows compiler bug with SFINAE functions.

        typedef Variant_RawVisitorHelper<void, VISITOR> Helper;


        return doApply<const Helper&>(Helper(&visitor), this->d_type);

    }

    template <class VISITOR>
    typename bsl::enable_if<
             Variant_ReturnValueHelper<VISITOR>::k_VaL == 0, void>::type
    applyRaw(const VISITOR& visitor) const {
        // Apply the specified 'visitor' to this variant by passing the value
        // this variant currently holds to 'visitor' object's 'operator()'.
        // This method does not return any values.  This method is selected
        // only if the templatized type 'VISITOR' does not define a 'typedef'
        // of 'ResultType' in its public interface.  The behavior is undefined
        // if the variant is unset.  Note that this method is declared in place
        // inline to workaround a windows compiler bug with SFINAE functions.


        typedef Variant_RawVisitorHelper<void, const VISITOR> Helper;

        return doApply<const Helper&>(Helper(&visitor), this->d_type);
    }

    template <class RET_TYPE, class VISITOR>
    RET_TYPE applyRaw(VISITOR& visitor) const;
    template <class RET_TYPE, class VISITOR>
    RET_TYPE applyRaw(const VISITOR& visitor) const;
        // Apply the specified 'visitor' to this variant by passing the value
        // this variant currently holds to 'visitor' object's 'operator()',
        // and return the value returned by the 'visitor' object's 'operator()'
        // of templatized 'RET_TYPE'.  The behavior is undefined if the current
        // state of the variant is unset.

    int maxSupportedBdexVersion() const;
        // Return the maximum supported version number of the type *currently*
        // held in this variant.  Note that this method is usually a *CLASS*
        // *METHOD*, but here it is an (instance) method, since the return
        // value depends on the actual type held in this instance.  See the
        // 'bdex' package-level documentation for more information on 'bdex'
        // streaming of value-semantic types and containers, and the
        // "'bdex' Streamability" section of the component-level documentation.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write this value to the specified output 'stream' using the
        // specified 'version' format and return a reference to the modifiable
        // 'stream'.  Note that 'version' is *not* used for the 'Variant'
        // object, but for the contained object, and thus has a different
        // meaning (and different value) depending on the variant type.  See
        // the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers, and the
        // "'bdex' Streamability" section of the component-level documentation.

    template <class TYPE>
    bool is() const;
        // Return 'true' if the value held by this variant object is of the
        // parameterized 'TYPE', and 'false' otherwise.  Note that 'TYPE' must
        // be specified explicitly, e.g., 'myValue.is<int>()'.

    bool isUnset() const;
        // Return 'true' if the variant is currently unset, and 'false'
        // otherwise.  An unset variant does not hold a value or type.  Note
        // that this method is preferred over checking the type index of the
        // variant.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the (absolute
        // value of) the optionally specified indentation 'level' and return a
        // reference to 'stream'.  If 'level' is specified, optionally specify
        // 'spacesPerLevel', the number of spaces per indentation level for
        // this and all of its nested objects.  If 'level' is negative,
        // suppress indentation of the first line.  If 'spacesPerLevel' is
        // negative, format the entire output on one line, suppressing all but
        // the initial indentation (as governed by 'level').  If 'stream' is
        // not valid on entry, this operation has no effect.  Note that the
        // string "(* UNPRINTABLE *)" will be printed if the object held by
        // this variant is not printable, and the string "(* NULL *)" will be
        // printed if this variant holds no object.

    template <class TYPE>
    const TYPE& the() const;
        // Return a reference to the non-modifiable value of the parameterized
        // 'TYPE' held by this variant object.  The behavior is undefined
        // unless 'is<TYPE>()' returns 'true' and 'TYPE' is not 'void'.  Note
        // that 'TYPE' must be specified explicitly, e.g.,
        // 'myValue.the<int>()'.

    int typeIndex() const;
        // Return the index in the list of 'TYPES' corresponding to the type of
        // the value currently held by this variant object (starting at 1), or
        // 0 if this object is unset.  Note that instead of switching code on
        // the type index, calling 'apply' is the preferred method of
        // manipulating different types stored inside a variant.

    const bsl::type_info& typeInfo() const;
        // Return 'typeid(void)'.
        //
        // DEPRECATED: Do not use.
};

// FREE OPERATORS
template <typename TYPES>
bool operator==(const VariantImp<TYPES>& lhs,
                const VariantImp<TYPES>& rhs);
    // Return 'true' if the specified 'lhs' variant object has the same value
    // as the specified 'rhs' variant object, and 'false' otherwise.  Two
    // variant objects have the same value if they are both set and hold
    // objects of the same type and same value, or are both unset.

template <typename TYPES>
bool operator!=(const VariantImp<TYPES>& lhs,
                const VariantImp<TYPES>& rhs);
    // Return 'true' if the specified 'lhs' variant object does not have the
    // same value as the specified 'rhs' variant object, and 'false' otherwise.
    // Two variant objects do not have the same value if one is set and the
    // other is unset, or if they are both set but hold objects that differ in
    // type or value.

template <class TYPES>
inline
bsl::ostream& operator<<(bsl::ostream&                  stream,
                         const VariantImp<TYPES>& rhs);
}  // close package namespace
    // Write the specified 'rhs' variant object to the specified output
    // 'stream' in a one-line (human-readable) format, and return a reference
    // to the modifiable 'stream'.

// FREE FUNCTIONS
template <typename TYPES>
void swap(bdlb::VariantImp<TYPES>& a, bdlb::VariantImp<TYPES>& b);

namespace bdlb {    // Swap the values of the specified 'a' and 'b' objects.  This method
    // provides the no-throw guarantee if the 'TYPE' template parameter has a
    // no-throw 'swap' and the two variant objects being swapped has the same
    // type.  Otherwise it provides the basic guarantee.

                       // ========================
                       // class Variant<...>
                       // ========================

template <class A1  = bslmf::Nil, class A2  = bslmf::Nil,
          class A3  = bslmf::Nil, class A4  = bslmf::Nil,
          class A5  = bslmf::Nil, class A6  = bslmf::Nil,
          class A7  = bslmf::Nil, class A8  = bslmf::Nil,
          class A9  = bslmf::Nil, class A10 = bslmf::Nil,
          class A11 = bslmf::Nil, class A12 = bslmf::Nil,
          class A13 = bslmf::Nil, class A14 = bslmf::Nil,
          class A15 = bslmf::Nil, class A16 = bslmf::Nil,
          class A17 = bslmf::Nil, class A18 = bslmf::Nil,
          class A19 = bslmf::Nil, class A20 = bslmf::Nil>
class Variant : public VariantImp<typename bslmf::TypeList<
                                          A1,  A2,  A3,  A4,  A5,
                                          A6,  A7,  A8,  A9,  A10,
                                          A11, A12, A13, A14, A15,
                                          A16, A17, A18, A19, A20>::ListType> {
    // This class provides a "variant" type, i.e., a type capable of storing
    // values from a list of parameterized 'A1' to 'A20' types.  Note that if
    // the number 'N' of types is smaller than 20, 'AN+1' up to 'A20' default
    // to 'bslmf::Nil', but it is more economical to use
    // 'VariantN' which accepts exactly 'N' template arguments, as this
    // leads to shorter symbols and debug string information.

    // TYPES
    typedef VariantImp<typename bslmf::TypeList<
                                       A1,  A2,  A3,  A4,  A5,
                                       A6,  A7,  A8,  A9,  A10,
                                       A11, A12, A13, A14, A15,
                                       A16, A17, A18, A19, A20>::ListType> Imp;

    typedef VariantImp_Traits<typename Imp::TypeList>          Traits;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant,
                                      bslma::UsesBslmaAllocator,
                                      Traits::VARIANT_USES_BSLMA_ALLOCATOR);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant,
                                      bsl::is_trivially_copyable,
                                      Traits::VARIANT_IS_BITWISE_COPYABLE);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant,
                                      bslmf::IsBitwiseMoveable,
                                      Traits::VARIANT_IS_BITWISE_MOVEABLE);
    BSLMF_NESTED_TRAIT_DECLARATION(Variant, bdlb::HasPrintMethod);

    // CREATORS
    Variant();
        // Create a variant object.  The type and value of this object are
        // unset.  Use the currently installed default allocator to supply
        // memory.

    template <class TYPE_OR_ALLOCATOR>
    explicit Variant(const TYPE_OR_ALLOCATOR& typeOrAllocator);
        // Create a variant object with the specified 'typeOrAlloc', which can
        // either be a type that the variant can hold, or an allocator to
        // supply memory.  If 'typeOrAlloc' is not a 'bslma::Allocator *', then
        // the variant will hold the value and type of 'typeOrAlloc', using
        // the currently installed default allocator to supply memory.
        // Otherwise, the variant will default construct to an unset type and
        // value using 'typeOrAlloc' to supply memory.  Note that this
        // templated constructor is created instead of creating an second
        // constructor that takes 'bslma::Allocator *' because templated
        // arguments are always a better match than child-to-base conversion.

    template <class TYPE>
    Variant(const TYPE&       value,
                  bslma::Allocator *basicAllocator);
        // Create a variant object having the specified 'value' and that uses
        // the specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Variant(const Variant&  original,
                  bslma::Allocator     *basicAllocator = 0);
        // Create a variant object having the type and value of the specified
        // 'original' variant.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    // MANIPULATORS
    template <class TYPE>
    Variant& operator=(const TYPE& value);
        // Assign to this object the specified 'value' of the specified 'TYPE',
        // and return a reference to this modifiable object.  The value
        // previously held by this variant (if any) will be destroyed if the
        // value's type is different from the parameterized 'TYPE'.

    Variant& operator=(const Variant& rhs);
        // Assign to this object the type and value currently hold by the
        // specified 'rhs' object, and return a reference to this modifiable
        // object.  The value previously held by this variant (if any) will be
        // destroyed if the value's type is different from the type held by the
        // 'rhs' object.
};

// CREATORS
template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17, class A18,
          class A19, class A20>
Variant<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12,
              A13, A14, A15, A16, A17, A18, A19, A20>::Variant()
{
}

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17, class A18,
          class A19, class A20>
template <class TYPE_OR_ALLOCATOR>
Variant<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12,
              A13, A14, A15, A16, A17, A18, A19, A20>::Variant(
                                          const TYPE_OR_ALLOCATOR& typeOrAlloc)
: Imp(typeOrAlloc)
{
}

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17, class A18,
          class A19, class A20>
template <class TYPE>
Variant<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12,
              A13, A14, A15, A16, A17, A18, A19, A20>::Variant(
                                              const TYPE&       value,
                                              bslma::Allocator *basicAllocator)
: Imp(value, basicAllocator)
{
}

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17, class A18,
          class A19, class A20>
Variant<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12,
              A13, A14, A15, A16, A17, A18, A19, A20>::Variant(
                                          const Variant&  original,
                                          bslma::Allocator     *basicAllocator)
: Imp(static_cast<const Imp &>(original), basicAllocator)
    // Up-cast needed since template matching has higher overloading precedence
    // than derived-to-base matching.
{
}

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17, class A18,
          class A19, class A20>
template <class TYPE>
inline
Variant<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12,
              A13, A14, A15, A16, A17, A18, A19, A20>&
Variant<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12,
              A13, A14, A15, A16, A17, A18, A19, A20>::operator=(
                                                             const TYPE& value)
{
    Imp::operator=(value);
    return *this;
}

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17, class A18,
          class A19, class A20>
inline
Variant<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12,
              A13, A14, A15, A16, A17, A18, A19, A20>&
Variant<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12,
              A13, A14, A15, A16, A17, A18, A19, A20>::operator=(
                                                   const Variant& rhs)
{
    // Up-cast needed since template matching has higher overloading precedence
    // than derived-to-base matching.

    Imp::operator=(static_cast<const Imp&>(rhs));
    return *this;
}

                       // =========================
                       // class Variant2<...>
                       // =========================

template <class A1, class A2>
class Variant2 : public VariantImp<typename bslmf::TypeList2<
                                                           A1, A2>::ListType> {
    // This class is a "specialization" of 'Variant' for a fixed number
    // (2) of types.  Its 2 template arguments *must* all be specified (none
    // are defaulted to 'bslmf::Nil').  It provides the same
    // functionality as 'Variant<A1, A2>'.

    // TYPES
    typedef VariantImp<typename bslmf::TypeList2<A1, A2>::ListType> Imp;
    typedef VariantImp_Traits<typename Imp::TypeList>             Traits;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant2,
                                      bslma::UsesBslmaAllocator,
                                      Traits::VARIANT_USES_BSLMA_ALLOCATOR);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant2,
                                      bsl::is_trivially_copyable,
                                      Traits::VARIANT_IS_BITWISE_COPYABLE);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant2,
                                      bslmf::IsBitwiseMoveable,
                                      Traits::VARIANT_IS_BITWISE_MOVEABLE);
    BSLMF_NESTED_TRAIT_DECLARATION(Variant2, bdlb::HasPrintMethod);

    // CREATORS
    Variant2();
        // Create a variant object.  The type and value of this object are
        // unset.  Use the currently installed default allocator to supply
        // memory.

    template <class TYPE_OR_ALLOCATOR>
    explicit Variant2(const TYPE_OR_ALLOCATOR& typeOrAllocator);
        // Create a variant object with the specified 'typeOrAlloc', which can
        // either be a type that the variant can hold, or an allocator to
        // supply memory.  If 'typeOrAlloc' is not a 'bslma::Allocator *', then
        // the variant will hold the value and type of 'typeOrAlloc', using
        // the currently installed default allocator to supply memory.
        // Otherwise, the variant will default construct the first specified
        // type in the type list using 'typeOrAlloc' to supply memory.  Note
        // that this templated constructor is created instead of creating an
        // second constructor that takes 'bslma::Allocator *' because templated
        // arguments are always a better match than child-to-base conversion.

    template <class TYPE>
    Variant2(const TYPE&       value,
                   bslma::Allocator *basicAllocator);
        // Create a variant object having the specified 'value' and that uses
        // the specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Variant2(const Variant2&  original,
                   bslma::Allocator      *basicAllocator = 0);
        // Create a variant object having the type and value of the specified
        // 'original' variant.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    // MANIPULATORS
    template <class TYPE>
    Variant2& operator=(const TYPE& value);
        // Assign to this object the specified 'value' of the specified 'TYPE',
        // and return a reference to this modifiable object.  The value
        // previously held by this variant (if any) will be destroyed if the
        // value's type is different from the parameterized 'TYPE'.

    Variant2& operator=(const Variant2& rhs);
        // Assign to this object the type and value currently hold by the
        // specified 'rhs' object, and return a reference to this modifiable
        // object.  The value previously held by this variant (if any) will be
        // destroyed if the value's type is different from the type held by the
        // 'rhs' object.
};

// CREATORS
template <class A1, class A2>
inline
Variant2<A1, A2>::Variant2()
{
}

template <class A1, class A2>
template <class TYPE_OR_ALLOCATOR>
inline
Variant2<A1, A2>::Variant2(const TYPE_OR_ALLOCATOR& typeOrAlloc)
: Imp(typeOrAlloc)
{
}

template <class A1, class A2>
template <class TYPE>
inline
Variant2<A1, A2>::Variant2(const TYPE&       value,
                                       bslma::Allocator *basicAllocator)
: Imp(value, basicAllocator)
{
}

template <class A1, class A2>
inline
Variant2<A1, A2>::Variant2(const Variant2&  original,
                                       bslma::Allocator      *basicAllocator)
: Imp(static_cast<const Imp&>(original), basicAllocator)
{
}

template <class A1, class A2>
template <class TYPE>
inline
Variant2<A1, A2>&
Variant2<A1, A2>::operator=(const TYPE& value)
{
    Imp::operator=(value);
    return *this;
}

template <class A1, class A2>
inline
Variant2<A1, A2>&
Variant2<A1, A2>::operator=(const Variant2& rhs)
{
    Imp::operator=(static_cast<const Imp&>(rhs));
    return *this;
}

                       // =========================
                       // class Variant3<...>
                       // =========================

template <class A1, class A2, class A3>
class Variant3 : public VariantImp<typename bslmf::TypeList3<
                                                       A1, A2, A3>::ListType> {
    // This class is a "specialization" of 'Variant' for a fixed number
    // (3) of types.  Its 3 template arguments *must* all be specified (none
    // are defaulted to 'bslmf::Nil').  It provides the same
    // functionality as 'Variant<A1, A2, A3>'.

    // TYPES
    typedef VariantImp<typename bslmf::TypeList3<
                                                    A1, A2, A3>::ListType> Imp;

    typedef VariantImp_Traits<typename Imp::TypeList>             Traits;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant3,
                                      bslma::UsesBslmaAllocator,
                                      Traits::VARIANT_USES_BSLMA_ALLOCATOR);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant3,
                                      bsl::is_trivially_copyable,
                                      Traits::VARIANT_IS_BITWISE_COPYABLE);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant3,
                                      bslmf::IsBitwiseMoveable,
                                      Traits::VARIANT_IS_BITWISE_MOVEABLE);
    BSLMF_NESTED_TRAIT_DECLARATION(Variant3, bdlb::HasPrintMethod);


    // CREATORS
    Variant3();
        // Create a variant object.  The type and value of this object are
        // unset.  Use the currently installed default allocator to supply
        // memory.

    template <class TYPE_OR_ALLOCATOR>
    explicit Variant3(const TYPE_OR_ALLOCATOR& typeOrAllocator);
        // Create a variant object with the specified 'typeOrAlloc', which can
        // either be a type that the variant can hold, or an allocator to
        // supply memory.  If 'typeOrAlloc' is not a 'bslma::Allocator *', then
        // the variant will hold the value and type of 'typeOrAlloc', using
        // the currently installed default allocator to supply memory.
        // Otherwise, the variant will default construct the first specified
        // type in the type list using 'typeOrAlloc' to supply memory.  Note
        // that this templated constructor is created instead of creating an
        // second constructor that takes 'bslma::Allocator *' because templated
        // arguments are always a better match than child-to-base conversion.

    template <class TYPE>
    Variant3(const TYPE&       value,
                   bslma::Allocator *basicAllocator);
        // Create a variant object having the specified 'value' and that uses
        // the specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Variant3(const Variant3&  original,
                   bslma::Allocator      *basicAllocator = 0);
        // Create a variant object having the type and value of the specified
        // 'original' variant.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    // MANIPULATORS
    template <class TYPE>
    Variant3& operator=(const TYPE& value);
        // Assign to this object the specified 'value' of the specified 'TYPE',
        // and return a reference to this modifiable object.  The value
        // previously held by this variant (if any) will be destroyed if the
        // value's type is different from the parameterized 'TYPE'.

    Variant3& operator=(const Variant3& rhs);
        // Assign to this object the type and value currently hold by the
        // specified 'rhs' object, and return a reference to this modifiable
        // object.  The value previously held by this variant (if any) will be
        // destroyed if the value's type is different from the type held by the
        // 'rhs' object.
};

// CREATORS
template <class A1, class A2, class A3>
inline
Variant3<A1, A2, A3>::Variant3()
{
}

template <class A1, class A2, class A3>
template <class TYPE_OR_ALLOCATOR>
inline
Variant3<A1, A2, A3>::
Variant3(const TYPE_OR_ALLOCATOR& typeOrAlloc)
: Imp(typeOrAlloc)
{
}

template <class A1, class A2, class A3>
template <class TYPE>
inline
Variant3<A1, A2, A3>::
Variant3(const TYPE&       value,
               bslma::Allocator *basicAllocator)
: Imp(value, basicAllocator)
{
}

template <class A1, class A2, class A3>
inline
Variant3<A1, A2, A3>::
Variant3(const Variant3&  original,
               bslma::Allocator      *basicAllocator)
: Imp(static_cast<const Imp&>(original), basicAllocator)
{
}

template <class A1, class A2, class A3>
template <class TYPE>
inline
Variant3<A1, A2, A3>&
Variant3<A1, A2, A3>::operator=(const TYPE& value)
{
    Imp::operator=(value);
    return *this;
}

template <class A1, class A2, class A3>
inline
Variant3<A1, A2, A3>&
Variant3<A1, A2, A3>::operator=(const Variant3& rhs)
{
    Imp::operator=(static_cast<const Imp&>(rhs));
    return *this;
}

                       // =========================
                       // class Variant4<...>
                       // =========================

template <class A1, class A2, class A3, class A4>
class Variant4 : public VariantImp<typename bslmf::TypeList4<
                                                   A1, A2, A3, A4>::ListType> {
    // This class is a "specialization" of 'Variant' for a fixed number
    // (4) of types.  Its 4 template arguments *must* all be specified (none
    // are defaulted to 'bslmf::Nil').  It provides the same
    // functionality as 'Variant<A1, A2, ..., A4>'.

    // TYPES
    typedef VariantImp<typename bslmf::TypeList4<
                                                A1, A2, A3, A4>::ListType> Imp;

    typedef VariantImp_Traits<typename Imp::TypeList>             Traits;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant4,
                                      bslma::UsesBslmaAllocator,
                                      Traits::VARIANT_USES_BSLMA_ALLOCATOR);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant4,
                                      bsl::is_trivially_copyable,
                                      Traits::VARIANT_IS_BITWISE_COPYABLE);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant4,
                                      bslmf::IsBitwiseMoveable,
                                      Traits::VARIANT_IS_BITWISE_MOVEABLE);
    BSLMF_NESTED_TRAIT_DECLARATION(Variant4, bdlb::HasPrintMethod);


    // CREATORS
    Variant4();
        // Create a variant object.  The type and value of this object are
        // unset.  Use the currently installed default allocator to supply
        // memory.

    template <class TYPE_OR_ALLOCATOR>
    explicit Variant4(const TYPE_OR_ALLOCATOR& typeOrAllocator);
        // Create a variant object with the specified 'typeOrAlloc', which can
        // either be a type that the variant can hold, or an allocator to
        // supply memory.  If 'typeOrAlloc' is not a 'bslma::Allocator *', then
        // the variant will hold the value and type of 'typeOrAlloc', using
        // the currently installed default allocator to supply memory.
        // Otherwise, the variant will default construct the first specified
        // type in the type list using 'typeOrAlloc' to supply memory.  Note
        // that this templated constructor is created instead of creating an
        // second constructor that takes 'bslma::Allocator *' because templated
        // arguments are always a better match than child-to-base conversion.

    template <class TYPE>
    Variant4(const TYPE&       value,
                   bslma::Allocator *basicAllocator);
        // Create a variant object having the specified 'value' and that uses
        // the specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Variant4(const Variant4&  original,
                   bslma::Allocator      *basicAllocator = 0);
        // Create a variant object having the type and value of the specified
        // 'original' variant.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    // MANIPULATORS
    template <class TYPE>
    Variant4& operator=(const TYPE& value);
        // Assign to this object the specified 'value' of the specified 'TYPE',
        // and return a reference to this modifiable object.  The value
        // previously held by this variant (if any) will be destroyed if the
        // value's type is different from the parameterized 'TYPE'.

    Variant4& operator=(const Variant4& rhs);
        // Assign to this object the type and value currently hold by the
        // specified 'rhs' object, and return a reference to this modifiable
        // object.  The value previously held by this variant (if any) will be
        // destroyed if the value's type is different from the type held by the
        // 'rhs' object.
};

// CREATORS
template <class A1, class A2, class A3, class A4>
inline
Variant4<A1, A2, A3, A4>::Variant4()
{
}

template <class A1, class A2, class A3, class A4>
template <class TYPE_OR_ALLOCATOR>
inline
Variant4<A1, A2, A3, A4>::
Variant4(const TYPE_OR_ALLOCATOR& typeOrAlloc)
: Imp(typeOrAlloc)
{
}

template <class A1, class A2, class A3, class A4>
template <class TYPE>
inline
Variant4<A1, A2, A3, A4>::
Variant4(const TYPE&       value,
               bslma::Allocator *basicAllocator)
: Imp(value, basicAllocator)
{
}

template <class A1, class A2, class A3, class A4>
inline
Variant4<A1, A2, A3, A4>::
Variant4(const Variant4&  original,
               bslma::Allocator      *basicAllocator)
: Imp(static_cast<const Imp&>(original), basicAllocator)
{
}

template <class A1, class A2, class A3, class A4>
template <class TYPE>
inline
Variant4<A1, A2, A3, A4>&
Variant4<A1, A2, A3, A4>::operator=(const TYPE& value)
{
    Imp::operator=(value);
    return *this;
}

template <class A1, class A2, class A3, class A4>
inline
Variant4<A1, A2, A3, A4>&
Variant4<A1, A2, A3, A4>::operator=(const Variant4& rhs)
{
    Imp::operator=(static_cast<const Imp&>(rhs));
    return *this;
}

                       // =========================
                       // class Variant5<...>
                       // =========================

template <class A1, class A2, class A3, class A4, class A5>
class Variant5 : public VariantImp<typename bslmf::TypeList5<
                                               A1, A2, A3, A4, A5>::ListType> {
    // This class is a "specialization" of 'Variant' for a fixed number
    // (5) of types.  Its 5 template arguments *must* all be specified (none
    // are defaulted to 'bslmf::Nil').  It provides the same
    // functionality as 'Variant<A1, A2, ..., A5>'.

    // TYPES
    typedef VariantImp<typename bslmf::TypeList5<
                                            A1, A2, A3, A4, A5>::ListType> Imp;

    typedef VariantImp_Traits<typename Imp::TypeList>             Traits;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant5,
                                      bslma::UsesBslmaAllocator,
                                      Traits::VARIANT_USES_BSLMA_ALLOCATOR);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant5,
                                      bsl::is_trivially_copyable,
                                      Traits::VARIANT_IS_BITWISE_COPYABLE);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant5,
                                      bslmf::IsBitwiseMoveable,
                                      Traits::VARIANT_IS_BITWISE_MOVEABLE);
    BSLMF_NESTED_TRAIT_DECLARATION(Variant5, bdlb::HasPrintMethod);


    // CREATORS
    Variant5();
        // Create a variant object.  The type and value of this object are
        // unset.  Use the currently installed default allocator to supply
        // memory.

    template <class TYPE_OR_ALLOCATOR>
    explicit Variant5(const TYPE_OR_ALLOCATOR& typeOrAllocator);
        // Create a variant object with the specified 'typeOrAlloc', which can
        // either be a type that the variant can hold, or an allocator to
        // supply memory.  If 'typeOrAlloc' is not a 'bslma::Allocator *', then
        // the variant will hold the value and type of 'typeOrAlloc', using
        // the currently installed default allocator to supply memory.
        // Otherwise, the variant will default construct the first specified
        // type in the type list using 'typeOrAlloc' to supply memory.  Note
        // that this templated constructor is created instead of creating an
        // second constructor that takes 'bslma::Allocator *' because templated
        // arguments are always a better match than child-to-base conversion.

    template <class TYPE>
    Variant5(const TYPE&       value,
                   bslma::Allocator *basicAllocator);
        // Create a variant object having the specified 'value' and that uses
        // the specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Variant5(const Variant5&  original,
                   bslma::Allocator      *basicAllocator = 0);
        // Create a variant object having the type and value of the specified
        // 'original' variant.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    // MANIPULATORS
    template <class TYPE>
    Variant5& operator=(const TYPE& value);
        // Assign to this object the specified 'value' of the specified 'TYPE',
        // and return a reference to this modifiable object.  The value
        // previously held by this variant (if any) will be destroyed if the
        // value's type is different from the parameterized 'TYPE'.

    Variant5& operator=(const Variant5& rhs);
        // Assign to this object the type and value currently hold by the
        // specified 'rhs' object, and return a reference to this modifiable
        // object.  The value previously held by this variant (if any) will be
        // destroyed if the value's type is different from the type held by the
        // 'rhs' object.
};

// CREATORS
template <class A1, class A2, class A3, class A4, class A5>
inline
Variant5<A1, A2, A3, A4, A5>::Variant5()
{
}

template <class A1, class A2, class A3, class A4, class A5>
template <class TYPE_OR_ALLOCATOR>
inline
Variant5<A1, A2, A3, A4, A5>::
Variant5(const TYPE_OR_ALLOCATOR& typeOrAlloc)
: Imp(typeOrAlloc)
{
}

template <class A1, class A2, class A3, class A4, class A5>
template <class TYPE>
inline
Variant5<A1, A2, A3, A4, A5>::
Variant5(const TYPE&       value,
               bslma::Allocator *basicAllocator)
: Imp(value, basicAllocator)
{
}

template <class A1, class A2, class A3, class A4, class A5>
inline
Variant5<A1, A2, A3, A4, A5>::
Variant5(const Variant5&  original,
               bslma::Allocator      *basicAllocator)
: Imp(static_cast<const Imp&>(original), basicAllocator)
{
}

template <class A1, class A2, class A3, class A4, class A5>
template <class TYPE>
inline
Variant5<A1, A2, A3, A4, A5>&
Variant5<A1, A2, A3, A4, A5>::operator=(const TYPE& value)
{
    Imp::operator=(value);
    return *this;
}

template <class A1, class A2, class A3, class A4, class A5>
inline
Variant5<A1, A2, A3, A4, A5>&
Variant5<A1, A2, A3, A4, A5>::operator=(const Variant5& rhs)
{
    Imp::operator=(static_cast<const Imp&>(rhs));
    return *this;
}

                       // =========================
                       // class Variant6<...>
                       // =========================

template <class A1, class A2, class A3, class A4, class A5, class A6>
class Variant6 : public VariantImp<typename bslmf::TypeList6<
                                           A1, A2, A3, A4, A5, A6>::ListType> {
    // This class is a "specialization" of 'Variant' for a fixed number
    // (6) of types.  Its 6 template arguments *must* all be specified (none
    // are defaulted to 'bslmf::Nil').  It provides the same
    // functionality as 'Variant<A1, A2, ..., A6>'.

    // TYPES
    typedef VariantImp<typename bslmf::TypeList6<
                                        A1, A2, A3, A4, A5, A6>::ListType> Imp;

    typedef VariantImp_Traits<typename Imp::TypeList>             Traits;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant6,
                                      bslma::UsesBslmaAllocator,
                                      Traits::VARIANT_USES_BSLMA_ALLOCATOR);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant6,
                                      bsl::is_trivially_copyable,
                                      Traits::VARIANT_IS_BITWISE_COPYABLE);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant6,
                                      bslmf::IsBitwiseMoveable,
                                      Traits::VARIANT_IS_BITWISE_MOVEABLE);
    BSLMF_NESTED_TRAIT_DECLARATION(Variant6, bdlb::HasPrintMethod);


    // CREATORS
    Variant6();
        // Create a variant object.  The type and value of this object are
        // unset.  Use the currently installed default allocator to supply
        // memory.

    template <class TYPE_OR_ALLOCATOR>
    explicit Variant6(const TYPE_OR_ALLOCATOR& typeOrAllocator);
        // Create a variant object with the specified 'typeOrAlloc', which can
        // either be a type that the variant can hold, or an allocator to
        // supply memory.  If 'typeOrAlloc' is not a 'bslma::Allocator *', then
        // the variant will hold the value and type of 'typeOrAlloc', using
        // the currently installed default allocator to supply memory.
        // Otherwise, the variant will default construct the first specified
        // type in the type list using 'typeOrAlloc' to supply memory.  Note
        // that this templated constructor is created instead of creating an
        // second constructor that takes 'bslma::Allocator *' because templated
        // arguments are always a better match than child-to-base conversion.

    template <class TYPE>
    Variant6(const TYPE&       value,
                   bslma::Allocator *basicAllocator);
        // Create a variant object having the specified 'value' and that uses
        // the specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Variant6(const Variant6&  original,
                   bslma::Allocator      *basicAllocator = 0);
        // Create a variant object having the type and value of the specified
        // 'original' variant.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    // MANIPULATORS
    template <class TYPE>
    Variant6& operator=(const TYPE& value);
        // Assign to this object the specified 'value' of the specified 'TYPE',
        // and return a reference to this modifiable object.  The value
        // previously held by this variant (if any) will be destroyed if the
        // value's type is different from the parameterized 'TYPE'.

    Variant6& operator=(const Variant6& rhs);
        // Assign to this object the type and value currently hold by the
        // specified 'rhs' object, and return a reference to this modifiable
        // object.  The value previously held by this variant (if any) will be
        // destroyed if the value's type is different from the type held by the
        // 'rhs' object.
};

// CREATORS
template <class A1, class A2, class A3, class A4, class A5, class A6>
inline
Variant6<A1, A2, A3, A4, A5, A6>::Variant6()
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6>
template <class TYPE_OR_ALLOCATOR>
inline
Variant6<A1, A2, A3, A4, A5, A6>::
Variant6(const TYPE_OR_ALLOCATOR& typeOrAlloc)
: Imp(typeOrAlloc)
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6>
template <class TYPE>
inline
Variant6<A1, A2, A3, A4, A5, A6>::
Variant6(const TYPE&       value,
               bslma::Allocator *basicAllocator)
: Imp(value, basicAllocator)
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6>
inline
Variant6<A1, A2, A3, A4, A5, A6>::
Variant6(const Variant6&  original,
               bslma::Allocator      *basicAllocator)
: Imp(static_cast<const Imp&>(original), basicAllocator)
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6>
template <class TYPE>
inline
Variant6<A1, A2, A3, A4, A5, A6>&
Variant6<A1, A2, A3, A4, A5, A6>::operator=(const TYPE& value)
{
    Imp::operator=(value);
    return *this;
}

template <class A1, class A2, class A3, class A4, class A5, class A6>
inline
Variant6<A1, A2, A3, A4, A5, A6>&
Variant6<A1, A2, A3, A4, A5, A6>::operator=(const Variant6& rhs)
{
    Imp::operator=(static_cast<const Imp&>(rhs));
    return *this;
}

                       // =========================
                       // class Variant7<...>
                       // =========================

template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7>
class Variant7 : public VariantImp<typename bslmf::TypeList7<
                                       A1, A2, A3, A4, A5, A6, A7>::ListType> {
    // This class is a "specialization" of 'Variant' for a fixed number
    // (7) of types.  Its 7 template arguments *must* all be specified (none
    // are defaulted to 'bslmf::Nil').  It provides the same
    // functionality as 'Variant<A1, A2, ..., A7>'.

    // TYPES
    typedef VariantImp<typename bslmf::TypeList7<A1, A2, A3, A4, A5,
                                                      A6, A7>::ListType> Imp;

    typedef VariantImp_Traits<typename Imp::TypeList>             Traits;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant7,
                                      bslma::UsesBslmaAllocator,
                                      Traits::VARIANT_USES_BSLMA_ALLOCATOR);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant7,
                                      bsl::is_trivially_copyable,
                                      Traits::VARIANT_IS_BITWISE_COPYABLE);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant7,
                                      bslmf::IsBitwiseMoveable,
                                      Traits::VARIANT_IS_BITWISE_MOVEABLE);
    BSLMF_NESTED_TRAIT_DECLARATION(Variant7, bdlb::HasPrintMethod);


    // CREATORS
    Variant7();
        // Create a variant object.  The type and value of this object are
        // unset.  Use the currently installed default allocator to supply
        // memory.

    template <class TYPE_OR_ALLOCATOR>
    explicit Variant7(const TYPE_OR_ALLOCATOR& typeOrAllocator);
        // Create a variant object with the specified 'typeOrAlloc', which can
        // either be a type that the variant can hold, or an allocator to
        // supply memory.  If 'typeOrAlloc' is not a 'bslma::Allocator *', then
        // the variant will hold the value and type of 'typeOrAlloc', using
        // the currently installed default allocator to supply memory.
        // Otherwise, the variant will default construct the first specified
        // type in the type list using 'typeOrAlloc' to supply memory.  Note
        // that this templated constructor is created instead of creating an
        // second constructor that takes 'bslma::Allocator *' because templated
        // arguments are always a better match than child-to-base conversion.

    template <class TYPE>
    Variant7(const TYPE&       value,
                   bslma::Allocator *basicAllocator);
        // Create a variant object having the specified 'value' and that uses
        // the specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Variant7(const Variant7&  original,
                   bslma::Allocator      *basicAllocator = 0);
        // Create a variant object having the type and value of the specified
        // 'original' variant.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    // MANIPULATORS
    template <class TYPE>
    Variant7& operator=(const TYPE& value);
        // Assign to this object the specified 'value' of the specified 'TYPE',
        // and return a reference to this modifiable object.  The value
        // previously held by this variant (if any) will be destroyed if the
        // value's type is different from the parameterized 'TYPE'.

    Variant7& operator=(const Variant7& rhs);
        // Assign to this object the type and value currently hold by the
        // specified 'rhs' object, and return a reference to this modifiable
        // object.  The value previously held by this variant (if any) will be
        // destroyed if the value's type is different from the type held by the
        // 'rhs' object.
};

// CREATORS
template <class A1, class A2, class A3, class A4, class A5, class A6, class A7>
inline
Variant7<A1, A2, A3, A4, A5, A6, A7>::Variant7()
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7>
template <class TYPE_OR_ALLOCATOR>
inline
Variant7<A1, A2, A3, A4, A5, A6, A7>::
Variant7(const TYPE_OR_ALLOCATOR& typeOrAlloc)
: Imp(typeOrAlloc)
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7>
template <class TYPE>
inline
Variant7<A1, A2, A3, A4, A5, A6, A7>::
Variant7(const TYPE&       value,
               bslma::Allocator *basicAllocator)
: Imp(value, basicAllocator)
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7>
inline
Variant7<A1, A2, A3, A4, A5, A6, A7>::
Variant7(const Variant7&  original,
               bslma::Allocator      *basicAllocator)
: Imp(static_cast<const Imp&>(original), basicAllocator)
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7>
template <class TYPE>
inline
Variant7<A1, A2, A3, A4, A5, A6, A7>&
Variant7<A1, A2, A3, A4, A5, A6, A7>::operator=(const TYPE& value)
{
    Imp::operator=(value);
    return *this;
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7>
inline
Variant7<A1, A2, A3, A4, A5, A6, A7>&
Variant7<A1, A2, A3, A4, A5, A6, A7>::
operator=(const Variant7& rhs)
{
    Imp::operator=(static_cast<const Imp&>(rhs));
    return *this;
}

                       // =========================
                       // class Variant8<...>
                       // =========================

template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8>
class Variant8 : public VariantImp<typename bslmf::TypeList8<
              A1, A2, A3, A4, A5, A6, A7, A8>::ListType> {
    // This class is a "specialization" of 'Variant' for a fixed number
    // (8) of types.  Its 8 template arguments *must* all be specified (none
    // are defaulted to 'bslmf::Nil').  It provides the same
    // functionality as 'Variant<A1, A2, ..., A8>'.

    // TYPES
    typedef VariantImp<typename bslmf::TypeList8<
                                                    A1, A2, A3, A4, A5,
                                                    A6, A7, A8>::ListType> Imp;

    typedef VariantImp_Traits<typename Imp::TypeList>             Traits;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant8,
                                      bslma::UsesBslmaAllocator,
                                      Traits::VARIANT_USES_BSLMA_ALLOCATOR);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant8,
                                      bsl::is_trivially_copyable,
                                      Traits::VARIANT_IS_BITWISE_COPYABLE);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant8,
                                      bslmf::IsBitwiseMoveable,
                                      Traits::VARIANT_IS_BITWISE_MOVEABLE);
    BSLMF_NESTED_TRAIT_DECLARATION(Variant8, bdlb::HasPrintMethod);


    // CREATORS
    Variant8();
        // Create a variant object.  The type and value of this object are
        // unset.  Use the currently installed default allocator to supply
        // memory.

    template <class TYPE_OR_ALLOCATOR>
    explicit Variant8(const TYPE_OR_ALLOCATOR& typeOrAllocator);
        // Create a variant object with the specified 'typeOrAlloc', which can
        // either be a type that the variant can hold, or an allocator to
        // supply memory.  If 'typeOrAlloc' is not a 'bslma::Allocator *', then
        // the variant will hold the value and type of 'typeOrAlloc', using
        // the currently installed default allocator to supply memory.
        // Otherwise, the variant will default construct the first specified
        // type in the type list using 'typeOrAlloc' to supply memory.  Note
        // that this templated constructor is created instead of creating an
        // second constructor that takes 'bslma::Allocator *' because templated
        // arguments are always a better match than child-to-base conversion.

    template <class TYPE>
    Variant8(const TYPE&       value,
                   bslma::Allocator *basicAllocator);
        // Create a variant object having the specified 'value' and that uses
        // the specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Variant8(const Variant8&  original,
                   bslma::Allocator      *basicAllocator = 0);
        // Create a variant object having the type and value of the specified
        // 'original' variant.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    // MANIPULATORS
    template <class TYPE>
    Variant8& operator=(const TYPE& value);
        // Assign to this object the specified 'value' of the specified 'TYPE',
        // and return a reference to this modifiable object.  The value
        // previously held by this variant (if any) will be destroyed if the
        // value's type is different from the parameterized 'TYPE'.

    Variant8& operator=(const Variant8& rhs);
        // Assign to this object the type and value currently hold by the
        // specified 'rhs' object, and return a reference to this modifiable
        // object.  The value previously held by this variant (if any) will be
        // destroyed if the value's type is different from the type held by the
        // 'rhs' object.
};

// CREATORS
template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8>
inline
Variant8<A1, A2, A3, A4, A5, A6, A7, A8>::Variant8()
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8>
template <class TYPE_OR_ALLOCATOR>
inline
Variant8<A1, A2, A3, A4, A5, A6, A7, A8>::
Variant8(const TYPE_OR_ALLOCATOR& typeOrAlloc)
: Imp(typeOrAlloc)
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8>
template <class TYPE>
inline
Variant8<A1, A2, A3, A4, A5, A6, A7, A8>::
Variant8(const TYPE&       value,
               bslma::Allocator *basicAllocator)
: Imp(value, basicAllocator)
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8>
inline
Variant8<A1, A2, A3, A4, A5, A6, A7, A8>::
Variant8(const Variant8&  original,
               bslma::Allocator      *basicAllocator)
: Imp(static_cast<const Imp&>(original), basicAllocator)
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8>
template <class TYPE>
inline
Variant8<A1, A2, A3, A4, A5, A6, A7, A8>&
Variant8<A1, A2, A3, A4, A5, A6, A7, A8>::operator=(const TYPE& value)
{
    Imp::operator=(value);
    return *this;
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8>
inline
Variant8<A1, A2, A3, A4, A5, A6, A7, A8>&
Variant8<A1, A2, A3, A4, A5, A6, A7, A8>::
operator=(const Variant8& rhs)
{
    Imp::operator=(static_cast<const Imp&>(rhs));
    return *this;
}

                       // =========================
                       // class Variant9<...>
                       // =========================

template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9>
class Variant9 : public VariantImp<typename bslmf::TypeList9<
              A1, A2, A3, A4, A5, A6, A7, A8, A9>::ListType> {
    // This class is a "specialization" of 'Variant' for a fixed number
    // (9) of types.  Its 9 template arguments *must* all be specified (none
    // are defaulted to 'bslmf::Nil').  It provides the same
    // functionality as 'Variant<A1, A2, ..., A9>'.

    // TYPES
    typedef VariantImp<typename bslmf::TypeList9<
                                                A1, A2, A3, A4, A5,
                                                A6, A7, A8, A9>::ListType> Imp;

    typedef VariantImp_Traits<typename Imp::TypeList>             Traits;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant9,
                                      bslma::UsesBslmaAllocator,
                                      Traits::VARIANT_USES_BSLMA_ALLOCATOR);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant9,
                                      bsl::is_trivially_copyable,
                                      Traits::VARIANT_IS_BITWISE_COPYABLE);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant9,
                                      bslmf::IsBitwiseMoveable,
                                      Traits::VARIANT_IS_BITWISE_MOVEABLE);
    BSLMF_NESTED_TRAIT_DECLARATION(Variant9, bdlb::HasPrintMethod);


    // CREATORS
    Variant9();
        // Create a variant object.  The type and value of this object are
        // unset.  Use the currently installed default allocator to supply
        // memory.

    template <class TYPE_OR_ALLOCATOR>
    explicit Variant9(const TYPE_OR_ALLOCATOR& typeOrAllocator);
        // Create a variant object with the specified 'typeOrAlloc', which can
        // either be a type that the variant can hold, or an allocator to
        // supply memory.  If 'typeOrAlloc' is not a 'bslma::Allocator *', then
        // the variant will hold the value and type of 'typeOrAlloc', using
        // the currently installed default allocator to supply memory.
        // Otherwise, the variant will default construct the first specified
        // type in the type list using 'typeOrAlloc' to supply memory.  Note
        // that this templated constructor is created instead of creating an
        // second constructor that takes 'bslma::Allocator *' because templated
        // arguments are always a better match than child-to-base conversion.

    template <class TYPE>
    Variant9(const TYPE&       value,
                   bslma::Allocator *basicAllocator);
        // Create a variant object having the specified 'value' and that uses
        // the specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Variant9(const Variant9&  original,
                   bslma::Allocator      *basicAllocator = 0);
        // Create a variant object having the type and value of the specified
        // 'original' variant.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    // MANIPULATORS
    template <class TYPE>
    Variant9& operator=(const TYPE& value);
        // Assign to this object the specified 'value' of the specified 'TYPE',
        // and return a reference to this modifiable object.  The value
        // previously held by this variant (if any) will be destroyed if the
        // value's type is different from the parameterized 'TYPE'.

    Variant9& operator=(const Variant9& rhs);
        // Assign to this object the type and value currently hold by the
        // specified 'rhs' object, and return a reference to this modifiable
        // object.  The value previously held by this variant (if any) will be
        // destroyed if the value's type is different from the type held by the
        // 'rhs' object.
};

// CREATORS
template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9>
inline
Variant9<A1, A2, A3, A4, A5, A6, A7, A8, A9>::Variant9()
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9>
template <class TYPE_OR_ALLOCATOR>
inline
Variant9<A1, A2, A3, A4, A5, A6, A7, A8, A9>::
Variant9(const TYPE_OR_ALLOCATOR& typeOrAlloc)
: Imp(typeOrAlloc)
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9>
template <class TYPE>
inline
Variant9<A1, A2, A3, A4, A5, A6, A7, A8, A9>::
Variant9(const TYPE&       value,
               bslma::Allocator *basicAllocator)
: Imp(value, basicAllocator)
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9>
inline
Variant9<A1, A2, A3, A4, A5, A6, A7, A8, A9>::
Variant9(const Variant9&  original,
               bslma::Allocator      *basicAllocator)
: Imp(static_cast<const Imp&>(original), basicAllocator)
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9>
template <class TYPE>
inline
Variant9<A1, A2, A3, A4, A5, A6, A7, A8, A9>&
Variant9<A1, A2, A3, A4, A5, A6, A7, A8, A9>
::operator=(const TYPE& value)
{
    Imp::operator=(value);
    return *this;
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9>
inline
Variant9<A1, A2, A3, A4, A5, A6, A7, A8, A9>&
Variant9<A1, A2, A3, A4, A5, A6, A7, A8, A9>::
operator=(const Variant9& rhs)
{
    Imp::operator=(static_cast<const Imp&>(rhs));
    return *this;
}

                       // ==========================
                       // class Variant10<...>
                       // ==========================

template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9, class A10>
class Variant10 : public VariantImp<typename bslmf::TypeList10<
                          A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>::ListType> {
    // This class is a "specialization" of 'Variant' for a fixed number
    // (10) of types.  Its 10 template arguments *must* all be specified (none
    // are defaulted to 'bslmf::Nil').  It provides the same
    // functionality as 'Variant<A1, A2, ..., A10>'.

    // TYPES
    typedef VariantImp<typename bslmf::TypeList10<
                                           A1, A2, A3, A4, A5,
                                           A6, A7, A8, A9, A10>::ListType> Imp;

    typedef VariantImp_Traits<typename Imp::TypeList>             Traits;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant10,
                                      bslma::UsesBslmaAllocator,
                                      Traits::VARIANT_USES_BSLMA_ALLOCATOR);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant10,
                                      bsl::is_trivially_copyable,
                                      Traits::VARIANT_IS_BITWISE_COPYABLE);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant10,
                                      bslmf::IsBitwiseMoveable,
                                      Traits::VARIANT_IS_BITWISE_MOVEABLE);
    BSLMF_NESTED_TRAIT_DECLARATION(Variant10, bdlb::HasPrintMethod);


    // CREATORS
    Variant10();
        // Create a variant object.  The type and value of this object are
        // unset.  Use the currently installed default allocator to supply
        // memory.

    template <class TYPE_OR_ALLOCATOR>
    explicit Variant10(const TYPE_OR_ALLOCATOR& typeOrAllocator);
        // Create a variant object with the specified 'typeOrAlloc', which can
        // either be a type that the variant can hold, or an allocator to
        // supply memory.  If 'typeOrAlloc' is not a 'bslma::Allocator *', then
        // the variant will hold the value and type of 'typeOrAlloc', using
        // the currently installed default allocator to supply memory.
        // Otherwise, the variant will default construct the first specified
        // type in the type list using 'typeOrAlloc' to supply memory.  Note
        // that this templated constructor is created instead of creating an
        // second constructor that takes 'bslma::Allocator *' because templated
        // arguments are always a better match than child-to-base conversion.

    template <class TYPE>
    Variant10(const TYPE&       value,
                    bslma::Allocator *basicAllocator);
        // Create a variant object having the specified 'value' and that uses
        // the specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Variant10(const Variant10&  original,
                    bslma::Allocator       *basicAllocator = 0);
        // Create a variant object having the type and value of the specified
        // 'original' variant.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    // MANIPULATORS
    template <class TYPE>
    Variant10& operator=(const TYPE& value);
        // Assign to this object the specified 'value' of the specified 'TYPE',
        // and return a reference to this modifiable object.  The value
        // previously held by this variant (if any) will be destroyed if the
        // value's type is different from the parameterized 'TYPE'.

    Variant10& operator=(const Variant10& rhs);
        // Assign to this object the type and value currently hold by the
        // specified 'rhs' object, and return a reference to this modifiable
        // object.  The value previously held by this variant (if any) will be
        // destroyed if the value's type is different from the type held by the
        // 'rhs' object.
};

// CREATORS
template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10>
inline
Variant10<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>::Variant10()
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10>
template <class TYPE_OR_ALLOCATOR>
inline
Variant10<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>::
Variant10(const TYPE_OR_ALLOCATOR& typeOrAlloc)
: Imp(typeOrAlloc)
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10>
template <class TYPE>
inline
Variant10<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>::
Variant10(const TYPE&       value,
                bslma::Allocator *basicAllocator)
: Imp(value, basicAllocator)
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10>
inline
Variant10<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>::
Variant10(const Variant10&  original,
                bslma::Allocator       *basicAllocator)
: Imp(static_cast<const Imp&>(original), basicAllocator)
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10>
template <class TYPE>
inline
Variant10<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>&
Variant10<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>
::operator=(const TYPE& value)
{
    Imp::operator=(value);
    return *this;
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10>
inline
Variant10<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>&
Variant10<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>::
operator=(const Variant10& rhs)
{
    Imp::operator=(static_cast<const Imp&>(rhs));
    return *this;
}

                       // ==========================
                       // class Variant11<...>
                       // ==========================

template <class A1, class A2, class A3, class A4,  class A5, class A6,
          class A7, class A8, class A9, class A10, class A11>
class Variant11 : public VariantImp<typename bslmf::TypeList11<
                     A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11>::ListType> {
    // This class is a "specialization" of 'Variant' for a fixed number
    // (11) of types.  Its 11 template arguments *must* all be specified (none
    // are defaulted to 'bslmf::Nil').  It provides the same
    // functionality as 'Variant<A1, A2, ..., A11>'.

    // TYPES
    typedef VariantImp<typename bslmf::TypeList11<A1, A2, A3, A4, A5,
                                                       A6, A7, A8, A9, A10,
                                                       A11>::ListType> Imp;

    typedef VariantImp_Traits<typename Imp::TypeList>             Traits;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant11,
                                      bslma::UsesBslmaAllocator,
                                      Traits::VARIANT_USES_BSLMA_ALLOCATOR);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant11,
                                      bsl::is_trivially_copyable,
                                      Traits::VARIANT_IS_BITWISE_COPYABLE);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant11,
                                      bslmf::IsBitwiseMoveable,
                                      Traits::VARIANT_IS_BITWISE_MOVEABLE);
    BSLMF_NESTED_TRAIT_DECLARATION(Variant11, bdlb::HasPrintMethod);


    // CREATORS
    Variant11();
        // Create a variant object.  The type and value of this object are
        // unset.  Use the currently installed default allocator to supply
        // memory.

    template <class TYPE_OR_ALLOCATOR>
    explicit Variant11(const TYPE_OR_ALLOCATOR& typeOrAllocator);
        // Create a variant object with the specified 'typeOrAlloc', which can
        // either be a type that the variant can hold, or an allocator to
        // supply memory.  If 'typeOrAlloc' is not a 'bslma::Allocator *', then
        // the variant will hold the value and type of 'typeOrAlloc', using
        // the currently installed default allocator to supply memory.
        // Otherwise, the variant will default construct the first specified
        // type in the type list using 'typeOrAlloc' to supply memory.  Note
        // that this templated constructor is created instead of creating an
        // second constructor that takes 'bslma::Allocator *' because templated
        // arguments are always a better match than child-to-base conversion.

    template <class TYPE>
    Variant11(const TYPE&       value,
                    bslma::Allocator *basicAllocator);
        // Create a variant object having the specified 'value' and that uses
        // the specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Variant11(const Variant11&  original,
                    bslma::Allocator       *basicAllocator = 0);
        // Create a variant object having the type and value of the specified
        // 'original' variant.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    // MANIPULATORS
    template <class TYPE>
    Variant11& operator=(const TYPE& value);
        // Assign to this object the specified 'value' of the specified 'TYPE',
        // and return a reference to this modifiable object.  The value
        // previously held by this variant (if any) will be destroyed if the
        // value's type is different from the parameterized 'TYPE'.

    Variant11& operator=(const Variant11& rhs);
        // Assign to this object the type and value currently hold by the
        // specified 'rhs' object, and return a reference to this modifiable
        // object.  The value previously held by this variant (if any) will be
        // destroyed if the value's type is different from the type held by the
        // 'rhs' object.
};

// CREATORS
template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10, class A11>
inline
Variant11<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11>::
Variant11()
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10, class A11>
template <class TYPE_OR_ALLOCATOR>
inline
Variant11<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11>::
Variant11(const TYPE_OR_ALLOCATOR& typeOrAlloc)
: Imp(typeOrAlloc)
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10, class A11>
template <class TYPE>
inline
Variant11<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11>::
Variant11(const TYPE&       value,
                bslma::Allocator *basicAllocator)
: Imp(value, basicAllocator)
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10, class A11>
inline
Variant11<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11>::
Variant11(const Variant11&  original,
                bslma::Allocator       *basicAllocator)
: Imp(static_cast<const Imp&>(original), basicAllocator)
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10, class A11>
template <class TYPE>
inline
Variant11<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11>&
Variant11<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11>
::operator=(const TYPE& value)
{
    Imp::operator=(value);
    return *this;
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10, class A11>
inline
Variant11<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11>&
Variant11<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11>::
operator=(const Variant11& rhs)
{
    Imp::operator=(static_cast<const Imp&>(rhs));
    return *this;
}

                       // ==========================
                       // class Variant12<...>
                       // ==========================

template <class A1, class A2, class A3, class A4,  class A5,  class A6,
          class A7, class A8, class A9, class A10, class A11, class A12>
class Variant12 : public VariantImp<typename bslmf::TypeList12<
              A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12>::ListType> {
    // This class is a "specialization" of 'Variant' for a fixed number
    // (12) of types.  Its 12 template arguments *must* all be specified (none
    // are defaulted to 'bslmf::Nil').  It provides the same
    // functionality as 'Variant<A1, A2, ..., A12>'.

    // TYPES
    typedef VariantImp<typename bslmf::TypeList12<
                                                      A1,  A2,  A3,  A4,  A5,
                                                      A6,  A7,  A8,  A9,  A10,
                                                      A11, A12>::ListType> Imp;

    typedef VariantImp_Traits<typename Imp::TypeList>             Traits;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant12,
                                      bslma::UsesBslmaAllocator,
                                      Traits::VARIANT_USES_BSLMA_ALLOCATOR);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant12,
                                      bsl::is_trivially_copyable,
                                      Traits::VARIANT_IS_BITWISE_COPYABLE);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant12,
                                      bslmf::IsBitwiseMoveable,
                                      Traits::VARIANT_IS_BITWISE_MOVEABLE);
    BSLMF_NESTED_TRAIT_DECLARATION(Variant12, bdlb::HasPrintMethod);


    // CREATORS
    Variant12();
        // Create a variant object.  The type and value of this object are
        // unset.  Use the currently installed default allocator to supply
        // memory.

    template <class TYPE_OR_ALLOCATOR>
    explicit Variant12(const TYPE_OR_ALLOCATOR& typeOrAllocator);
        // Create a variant object with the specified 'typeOrAlloc', which can
        // either be a type that the variant can hold, or an allocator to
        // supply memory.  If 'typeOrAlloc' is not a 'bslma::Allocator *', then
        // the variant will hold the value and type of 'typeOrAlloc', using
        // the currently installed default allocator to supply memory.
        // Otherwise, the variant will default construct the first specified
        // type in the type list using 'typeOrAlloc' to supply memory.  Note
        // that this templated constructor is created instead of creating an
        // second constructor that takes 'bslma::Allocator *' because templated
        // arguments are always a better match than child-to-base conversion.

    template <class TYPE>
    Variant12(const TYPE&       value,
                    bslma::Allocator *basicAllocator);
        // Create a variant object having the specified 'value' and that uses
        // the specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Variant12(const Variant12&  original,
                    bslma::Allocator       *basicAllocator = 0);
        // Create a variant object having the type and value of the specified
        // 'original' variant.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    // MANIPULATORS
    template <class TYPE>
    Variant12& operator=(const TYPE& value);
        // Assign to this object the specified 'value' of the specified 'TYPE',
        // and return a reference to this modifiable object.  The value
        // previously held by this variant (if any) will be destroyed if the
        // value's type is different from the parameterized 'TYPE'.

    Variant12& operator=(const Variant12& rhs);
        // Assign to this object the type and value currently hold by the
        // specified 'rhs' object, and return a reference to this modifiable
        // object.  The value previously held by this variant (if any) will be
        // destroyed if the value's type is different from the type held by the
        // 'rhs' object.
};

// CREATORS
template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10, class A11, class A12>
inline
Variant12<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12>::
Variant12()
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10, class A11, class A12>
template <class TYPE_OR_ALLOCATOR>
inline
Variant12<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12>::
Variant12(const TYPE_OR_ALLOCATOR& typeOrAlloc)
: Imp(typeOrAlloc)
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10, class A11, class A12>
template <class TYPE>
inline
Variant12<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12>::
Variant12(const TYPE&       value,
                bslma::Allocator *basicAllocator)
: Imp(value, basicAllocator)
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10, class A11, class A12>
inline
Variant12<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12>::
Variant12(const Variant12&  original,
                bslma::Allocator       *basicAllocator)
: Imp(static_cast<const Imp&>(original), basicAllocator)
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10, class A11, class A12>
template <class TYPE>
inline
Variant12<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12>&
Variant12<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12>
::operator=(const TYPE& value)
{
    Imp::operator=(value);
    return *this;
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10, class A11, class A12>
inline
Variant12<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12>&
Variant12<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12>::
operator=(const Variant12& rhs)
{
    Imp::operator=(static_cast<const Imp&>(rhs));
    return *this;
}

                       // ==========================
                       // class Variant13<...>
                       // ==========================

template <class A1, class A2, class A3, class A4,  class A5,  class A6,
          class A7, class A8, class A9, class A10, class A11, class A12,
          class A13>
class Variant13 : public VariantImp<typename bslmf::TypeList13<
           A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13>::ListType> {
    // This class is a "specialization" of 'Variant' for a fixed number
    // (13) of types.  Its 13 template arguments *must* all be specified (none
    // are defaulted to 'bslmf::Nil').  It provides the same
    // functionality as 'Variant<A1, A2, ..., A13>'.

    // TYPES
    typedef VariantImp<typename bslmf::TypeList13<
                                                 A1,  A2,  A3,  A4,  A5,
                                                 A6,  A7,  A8,  A9,  A10,
                                                 A11, A12, A13>::ListType> Imp;

    typedef VariantImp_Traits<typename Imp::TypeList>             Traits;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant13,
                                      bslma::UsesBslmaAllocator,
                                      Traits::VARIANT_USES_BSLMA_ALLOCATOR);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant13,
                                      bsl::is_trivially_copyable,
                                      Traits::VARIANT_IS_BITWISE_COPYABLE);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant13,
                                      bslmf::IsBitwiseMoveable,
                                      Traits::VARIANT_IS_BITWISE_MOVEABLE);
    BSLMF_NESTED_TRAIT_DECLARATION(Variant13, bdlb::HasPrintMethod);


    // CREATORS
    Variant13();
        // Create a variant object.  The type and value of this object are
        // unset.  Use the currently installed default allocator to supply
        // memory.

    template <class TYPE_OR_ALLOCATOR>
    explicit Variant13(const TYPE_OR_ALLOCATOR& typeOrAllocator);
        // Create a variant object with the specified 'typeOrAlloc', which can
        // either be a type that the variant can hold, or an allocator to
        // supply memory.  If 'typeOrAlloc' is not a 'bslma::Allocator *', then
        // the variant will hold the value and type of 'typeOrAlloc', using
        // the currently installed default allocator to supply memory.
        // Otherwise, the variant will default construct the first specified
        // type in the type list using 'typeOrAlloc' to supply memory.  Note
        // that this templated constructor is created instead of creating an
        // second constructor that takes 'bslma::Allocator *' because templated
        // arguments are always a better match than child-to-base conversion.

    template <class TYPE>
    Variant13(const TYPE&       value,
                    bslma::Allocator *basicAllocator);
        // Create a variant object having the specified 'value' and that uses
        // the specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Variant13(const Variant13&  original,
                    bslma::Allocator       *basicAllocator = 0);
        // Create a variant object having the type and value of the specified
        // 'original' variant.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    // MANIPULATORS
    template <class TYPE>
    Variant13& operator=(const TYPE& value);
        // Assign to this object the specified 'value' of the specified 'TYPE',
        // and return a reference to this modifiable object.  The value
        // previously held by this variant (if any) will be destroyed if the
        // value's type is different from the parameterized 'TYPE'.

    Variant13& operator=(const Variant13& rhs);
        // Assign to this object the type and value currently hold by the
        // specified 'rhs' object, and return a reference to this modifiable
        // object.  The value previously held by this variant (if any) will be
        // destroyed if the value's type is different from the type held by the
        // 'rhs' object.
};

// CREATORS
template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10, class A11, class A12, class A13>
inline
Variant13<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13>::
Variant13()
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10, class A11, class A12, class A13>
template <class TYPE_OR_ALLOCATOR>
inline
Variant13<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13>::
Variant13(const TYPE_OR_ALLOCATOR& typeOrAlloc)
: Imp(typeOrAlloc)
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10, class A11, class A12, class A13>
template <class TYPE>
inline
Variant13<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13>::
Variant13(const TYPE&       value,
                bslma::Allocator *basicAllocator)
: Imp(value, basicAllocator)
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10, class A11, class A12, class A13>
inline
Variant13<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13>::
Variant13(const Variant13&  original,
                bslma::Allocator       *basicAllocator)
: Imp(static_cast<const Imp&>(original), basicAllocator)
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10, class A11, class A12, class A13>
template <class TYPE>
inline
Variant13<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13>&
Variant13<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13>
::operator=(const TYPE& value)
{
    Imp::operator=(value);
    return *this;
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10, class A11, class A12, class A13>
inline
Variant13<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13>&
Variant13<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13>::
operator=(const Variant13& rhs)
{
    Imp::operator=(static_cast<const Imp&>(rhs));
    return *this;
}

                       // ==========================
                       // class Variant14<...>
                       // ==========================

template <class A1,  class A2, class A3, class A4,  class A5,  class A6,
          class A7,  class A8, class A9, class A10, class A11, class A12,
          class A13, class A14>
class Variant14 : public VariantImp<typename bslmf::TypeList14<
                                               A1,  A2,  A3,  A4,  A5,
                                               A6,  A7,  A8,  A9,  A10,
                                               A11, A12, A13, A14>::ListType> {
    // This class is a "specialization" of 'Variant' for a fixed number
    // (14) of types.  Its 14 template arguments *must* all be specified (none
    // are defaulted to 'bslmf::Nil').  It provides the same
    // functionality as 'Variant<A1, A2, ..., A14>'.

    // TYPES
    typedef VariantImp<typename bslmf::TypeList14<
                                            A1,  A2,  A3,  A4,  A5,
                                            A6,  A7,  A8,  A9,  A10,
                                            A11, A12, A13, A14>::ListType> Imp;

    typedef VariantImp_Traits<typename Imp::TypeList>             Traits;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant14,
                                      bslma::UsesBslmaAllocator,
                                      Traits::VARIANT_USES_BSLMA_ALLOCATOR);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant14,
                                      bsl::is_trivially_copyable,
                                      Traits::VARIANT_IS_BITWISE_COPYABLE);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant14,
                                      bslmf::IsBitwiseMoveable,
                                      Traits::VARIANT_IS_BITWISE_MOVEABLE);
    BSLMF_NESTED_TRAIT_DECLARATION(Variant14, bdlb::HasPrintMethod);


    // CREATORS
    Variant14();
        // Create a variant object.  The type and value of this object are
        // unset.  Use the currently installed default allocator to supply
        // memory.

    template <class TYPE_OR_ALLOCATOR>
    explicit Variant14(const TYPE_OR_ALLOCATOR& typeOrAllocator);
        // Create a variant object with the specified 'typeOrAlloc', which can
        // either be a type that the variant can hold, or an allocator to
        // supply memory.  If 'typeOrAlloc' is not a 'bslma::Allocator *', then
        // the variant will hold the value and type of 'typeOrAlloc', using
        // the currently installed default allocator to supply memory.
        // Otherwise, the variant will default construct the first specified
        // type in the type list using 'typeOrAlloc' to supply memory.  Note
        // that this templated constructor is created instead of creating an
        // second constructor that takes 'bslma::Allocator *' because templated
        // arguments are always a better match than child-to-base conversion.

    template <class TYPE>
    Variant14(const TYPE&       value,
                    bslma::Allocator *basicAllocator);
        // Create a variant object having the specified 'value' and that uses
        // the specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Variant14(const Variant14&  original,
                    bslma::Allocator       *basicAllocator = 0);
        // Create a variant object having the type and value of the specified
        // 'original' variant.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    // MANIPULATORS
    template <class TYPE>
    Variant14& operator=(const TYPE& value);
        // Assign to this object the specified 'value' of the specified 'TYPE',
        // and return a reference to this modifiable object.  The value
        // previously held by this variant (if any) will be destroyed if the
        // value's type is different from the parameterized 'TYPE'.

    Variant14& operator=(const Variant14& rhs);
        // Assign to this object the type and value currently hold by the
        // specified 'rhs' object, and return a reference to this modifiable
        // object.  The value previously held by this variant (if any) will be
        // destroyed if the value's type is different from the type held by the
        // 'rhs' object.
};

// CREATORS
template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10, class A11, class A12, class A13,
          class A14>
inline
Variant14<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14>::
Variant14()
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10, class A11, class A12, class A13,
          class A14>
template <class TYPE_OR_ALLOCATOR>
inline
Variant14<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14>::
Variant14(const TYPE_OR_ALLOCATOR& typeOrAlloc)
: Imp(typeOrAlloc)
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10, class A11, class A12, class A13,
          class A14>
template <class TYPE>
inline
Variant14<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14>::
Variant14(const TYPE&       value,
                bslma::Allocator *basicAllocator)
: Imp(value, basicAllocator)
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10, class A11, class A12, class A13,
          class A14>
inline
Variant14<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14>::
Variant14(const Variant14&  original,
                bslma::Allocator       *basicAllocator)
: Imp(static_cast<const Imp&>(original), basicAllocator)
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10, class A11, class A12, class A13,
          class A14>
template <class TYPE>
inline
Variant14<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14>&
Variant14<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14>
::operator=(const TYPE& value)
{
    Imp::operator=(value);
    return *this;
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10, class A11, class A12, class A13,
          class A14>
inline
Variant14<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14>&
Variant14<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14>::
operator=(const Variant14& rhs)
{
    Imp::operator=(static_cast<const Imp&>(rhs));
    return *this;
}

                       // ==========================
                       // class Variant15<...>
                       // ==========================

template <class A1,  class A2,  class A3, class A4,  class A5,  class A6,
          class A7,  class A8,  class A9, class A10, class A11, class A12,
          class A13, class A14, class A15>
class Variant15 : public VariantImp<typename bslmf::TypeList15<
                                          A1,  A2,  A3,  A4,  A5,
                                          A6,  A7,  A8,  A9,  A10,
                                          A11, A12, A13, A14, A15>::ListType> {
    // This class is a "specialization" of 'Variant' for a fixed number
    // (15) of types.  Its 15 template arguments *must* all be specified (none
    // are defaulted to 'bslmf::Nil').  It provides the same
    // functionality as 'Variant<A1, A2, ..., A15>'.

    // TYPES
    typedef VariantImp<typename bslmf::TypeList15<
                                       A1,  A2,  A3,  A4,  A5,
                                       A6,  A7,  A8,  A9,  A10,
                                       A11, A12, A13, A14, A15>::ListType> Imp;

    typedef VariantImp_Traits<typename Imp::TypeList>             Traits;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant15,
                                      bslma::UsesBslmaAllocator,
                                      Traits::VARIANT_USES_BSLMA_ALLOCATOR);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant15,
                                      bsl::is_trivially_copyable,
                                      Traits::VARIANT_IS_BITWISE_COPYABLE);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant15,
                                      bslmf::IsBitwiseMoveable,
                                      Traits::VARIANT_IS_BITWISE_MOVEABLE);
    BSLMF_NESTED_TRAIT_DECLARATION(Variant15, bdlb::HasPrintMethod);


    // CREATORS
    Variant15();
        // Create a variant object.  The type and value of this object are
        // unset.  Use the currently installed default allocator to supply
        // memory.

    template <class TYPE_OR_ALLOCATOR>
    explicit Variant15(const TYPE_OR_ALLOCATOR& typeOrAllocator);
        // Create a variant object with the specified 'typeOrAlloc', which can
        // either be a type that the variant can hold, or an allocator to
        // supply memory.  If 'typeOrAlloc' is not a 'bslma::Allocator *', then
        // the variant will hold the value and type of 'typeOrAlloc', using
        // the currently installed default allocator to supply memory.
        // Otherwise, the variant will default construct the first specified
        // type in the type list using 'typeOrAlloc' to supply memory.  Note
        // that this templated constructor is created instead of creating an
        // second constructor that takes 'bslma::Allocator *' because templated
        // arguments are always a better match than child-to-base conversion.

    template <class TYPE>
    Variant15(const TYPE&       value,
                    bslma::Allocator *basicAllocator);
        // Create a variant object having the specified 'value' and that uses
        // the specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Variant15(const Variant15&  original,
                    bslma::Allocator       *basicAllocator = 0);
        // Create a variant object having the type and value of the specified
        // 'original' variant.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    // MANIPULATORS
    template <class TYPE>
    Variant15& operator=(const TYPE& value);
        // Assign to this object the specified 'value' of the specified 'TYPE',
        // and return a reference to this modifiable object.  The value
        // previously held by this variant (if any) will be destroyed if the
        // value's type is different from the parameterized 'TYPE'.

    Variant15& operator=(const Variant15& rhs);
        // Assign to this object the type and value currently hold by the
        // specified 'rhs' object, and return a reference to this modifiable
        // object.  The value previously held by this variant (if any) will be
        // destroyed if the value's type is different from the type held by the
        // 'rhs' object.
};

// CREATORS
template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10, class A11, class A12, class A13,
          class A14, class A15>
inline
Variant15<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
                A15>::
Variant15()
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10, class A11, class A12, class A13,
          class A14, class A15>
template <class TYPE_OR_ALLOCATOR>
inline
Variant15<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
                A15>::
Variant15(const TYPE_OR_ALLOCATOR& typeOrAlloc)
: Imp(typeOrAlloc)
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10, class A11, class A12, class A13,
          class A14, class A15>
template <class TYPE>
inline
Variant15<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
                A15>::
Variant15(const TYPE&       value,
                bslma::Allocator *basicAllocator)
: Imp(value, basicAllocator)
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10, class A11, class A12, class A13,
          class A14, class A15>
inline
Variant15<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
                A15>::
Variant15(const Variant15&  original,
                bslma::Allocator       *basicAllocator)
: Imp(static_cast<const Imp&>(original), basicAllocator)
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10, class A11, class A12, class A13,
          class A14, class A15>
template <class TYPE>
inline
Variant15<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
                A15>&
Variant15<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
                A15>
::operator=(const TYPE& value)
{
    Imp::operator=(value);
    return *this;
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10, class A11, class A12, class A13,
          class A14, class A15>
inline
Variant15<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
                A15>&
Variant15<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
                A15>::
operator=(const Variant15& rhs)
{
    Imp::operator=(static_cast<const Imp&>(rhs));
    return *this;
}

                       // ==========================
                       // class Variant16<...>
                       // ==========================

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16>
class Variant16 : public VariantImp<typename bslmf::TypeList16<
                                                       A1,  A2,  A3,  A4,  A5,
                                                       A6,  A7,  A8,  A9,  A10,
                                                       A11, A12, A13, A14, A15,
                                                       A16>::ListType> {
    // This class is a "specialization" of 'Variant' for a fixed number
    // (16) of types.  Its 16 template arguments *must* all be specified (none
    // are defaulted to 'bslmf::Nil').  It provides the same
    // functionality as 'Variant<A1, A2, ..., A16>'.

    // TYPES
    typedef VariantImp<typename bslmf::TypeList16<A1,  A2,  A3,  A4,  A5,
                                                       A6,  A7,  A8,  A9,  A10,
                                                       A11, A12, A13, A14, A15,
                                                       A16>::ListType> Imp;

    typedef VariantImp_Traits<typename Imp::TypeList>             Traits;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant16,
                                      bslma::UsesBslmaAllocator,
                                      Traits::VARIANT_USES_BSLMA_ALLOCATOR);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant16,
                                      bsl::is_trivially_copyable,
                                      Traits::VARIANT_IS_BITWISE_COPYABLE);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant16,
                                      bslmf::IsBitwiseMoveable,
                                      Traits::VARIANT_IS_BITWISE_MOVEABLE);
    BSLMF_NESTED_TRAIT_DECLARATION(Variant16, bdlb::HasPrintMethod);


    // CREATORS
    Variant16();
        // Create a variant object.  The type and value of this object are
        // unset.  Use the currently installed default allocator to supply
        // memory.

    template <class TYPE_OR_ALLOCATOR>
    explicit Variant16(const TYPE_OR_ALLOCATOR& typeOrAllocator);
        // Create a variant object with the specified 'typeOrAlloc', which can
        // either be a type that the variant can hold, or an allocator to
        // supply memory.  If 'typeOrAlloc' is not a 'bslma::Allocator *', then
        // the variant will hold the value and type of 'typeOrAlloc', using
        // the currently installed default allocator to supply memory.
        // Otherwise, the variant will default construct the first specified
        // type in the type list using 'typeOrAlloc' to supply memory.  Note
        // that this templated constructor is created instead of creating an
        // second constructor that takes 'bslma::Allocator *' because templated
        // arguments are always a better match than child-to-base conversion.

    template <class TYPE>
    Variant16(const TYPE&       value,
                    bslma::Allocator *basicAllocator);
        // Create a variant object having the specified 'value' and that uses
        // the specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Variant16(const Variant16&  original,
                    bslma::Allocator       *basicAllocator = 0);
        // Create a variant object having the type and value of the specified
        // 'original' variant.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    // MANIPULATORS
    template <class TYPE>
    Variant16& operator=(const TYPE& value);
        // Assign to this object the specified 'value' of the specified 'TYPE',
        // and return a reference to this modifiable object.  The value
        // previously held by this variant (if any) will be destroyed if the
        // value's type is different from the parameterized 'TYPE'.

    Variant16& operator=(const Variant16& rhs);
        // Assign to this object the type and value currently hold by the
        // specified 'rhs' object, and return a reference to this modifiable
        // object.  The value previously held by this variant (if any) will be
        // destroyed if the value's type is different from the type held by the
        // 'rhs' object.
};

// CREATORS
template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10, class A11, class A12, class A13,
          class A14, class A15, class A16>
inline
Variant16<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
                A15, A16>::
Variant16()
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10, class A11, class A12, class A13,
          class A14, class A15, class A16>
template <class TYPE_OR_ALLOCATOR>
inline
Variant16<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
                A15, A16>::
Variant16(const TYPE_OR_ALLOCATOR& typeOrAlloc)
: Imp(typeOrAlloc)
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10, class A11, class A12, class A13,
          class A14, class A15, class A16>
template <class TYPE>
inline
Variant16<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
                A15, A16>::
Variant16(const TYPE&       value,
                bslma::Allocator *basicAllocator)
: Imp(value, basicAllocator)
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10, class A11, class A12, class A13,
          class A14, class A15, class A16>
inline
Variant16<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
                A15, A16>::
Variant16(const Variant16&  original,
                bslma::Allocator       *basicAllocator)
: Imp(static_cast<const Imp&>(original), basicAllocator)
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10, class A11, class A12, class A13,
          class A14, class A15, class A16>
template <class TYPE>
inline
Variant16<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
                A15, A16>&
Variant16<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
                A15, A16>
::operator=(const TYPE& value)
{
    Imp::operator=(value);
    return *this;
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10, class A11, class A12, class A13,
          class A14, class A15, class A16>
inline
Variant16<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
                A15, A16>&
Variant16<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
                A15, A16>::
operator=(const Variant16& rhs)
{
    Imp::operator=(static_cast<const Imp&>(rhs));
    return *this;
}

                       // ==========================
                       // class Variant17<...>
                       // ==========================

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17>
class Variant17 : public VariantImp<typename bslmf::TypeList17<
                                                       A1,  A2,  A3,  A4,  A5,
                                                       A6,  A7,  A8,  A9,  A10,
                                                       A11, A12, A13, A14, A15,
                                                       A16, A17>::ListType> {
    // This class is a "specialization" of 'Variant' for a fixed number
    // (17) of types.  Its 17 template arguments *must* all be specified (none
    // are defaulted to 'bslmf::Nil').  It provides the same
    // functionality as 'Variant<A1, A2, ..., A17>'.

    // TYPES
    typedef VariantImp<typename bslmf::TypeList17<
                                                      A1,  A2,  A3,  A4,  A5,
                                                      A6,  A7,  A8,  A9,  A10,
                                                      A11, A12, A13, A14, A15,
                                                      A16, A17>::ListType> Imp;

    typedef VariantImp_Traits<typename Imp::TypeList>             Traits;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant17,
                                      bslma::UsesBslmaAllocator,
                                      Traits::VARIANT_USES_BSLMA_ALLOCATOR);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant17,
                                      bsl::is_trivially_copyable,
                                      Traits::VARIANT_IS_BITWISE_COPYABLE);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant17,
                                      bslmf::IsBitwiseMoveable,
                                      Traits::VARIANT_IS_BITWISE_MOVEABLE);
    BSLMF_NESTED_TRAIT_DECLARATION(Variant17, bdlb::HasPrintMethod);


    // CREATORS
    Variant17();
        // Create a variant object.  The type and value of this object are
        // unset.  Use the currently installed default allocator to supply
        // memory.

    template <class TYPE_OR_ALLOCATOR>
    explicit Variant17(const TYPE_OR_ALLOCATOR& typeOrAllocator);
        // Create a variant object with the specified 'typeOrAlloc', which can
        // either be a type that the variant can hold, or an allocator to
        // supply memory.  If 'typeOrAlloc' is not a 'bslma::Allocator *', then
        // the variant will hold the value and type of 'typeOrAlloc', using
        // the currently installed default allocator to supply memory.
        // Otherwise, the variant will default construct the first specified
        // type in the type list using 'typeOrAlloc' to supply memory.  Note
        // that this templated constructor is created instead of creating an
        // second constructor that takes 'bslma::Allocator *' because templated
        // arguments are always a better match than child-to-base conversion.

    template <class TYPE>
    Variant17(const TYPE&       value,
                    bslma::Allocator *basicAllocator);
        // Create a variant object having the specified 'value' and that uses
        // the specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Variant17(const Variant17&  original,
                    bslma::Allocator       *basicAllocator = 0);
        // Create a variant object having the type and value of the specified
        // 'original' variant.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    // MANIPULATORS
    template <class TYPE>
    Variant17& operator=(const TYPE& value);
        // Assign to this object the specified 'value' of the specified 'TYPE',
        // and return a reference to this modifiable object.  The value
        // previously held by this variant (if any) will be destroyed if the
        // value's type is different from the parameterized 'TYPE'.

    Variant17& operator=(const Variant17& rhs);
        // Assign to this object the type and value currently hold by the
        // specified 'rhs' object, and return a reference to this modifiable
        // object.  The value previously held by this variant (if any) will be
        // destroyed if the value's type is different from the type held by the
        // 'rhs' object.
};

// CREATORS
template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10, class A11, class A12, class A13,
          class A14, class A15, class A16, class A17>
inline
Variant17<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
                A15, A16, A17>::
Variant17()
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10, class A11, class A12, class A13,
          class A14, class A15, class A16, class A17>
template <class TYPE_OR_ALLOCATOR>
inline
Variant17<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
                A15, A16, A17>::
Variant17(const TYPE_OR_ALLOCATOR& typeOrAlloc)
: Imp(typeOrAlloc)
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10, class A11, class A12, class A13,
          class A14, class A15, class A16, class A17>
template <class TYPE>
inline
Variant17<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
                A15, A16, A17>::
Variant17(const TYPE&       value,
                bslma::Allocator *basicAllocator)
: Imp(value, basicAllocator)
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10, class A11, class A12, class A13,
          class A14, class A15, class A16, class A17>
inline
Variant17<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
                A15, A16, A17>::
Variant17(const Variant17&  original,
                bslma::Allocator       *basicAllocator)
: Imp(static_cast<const Imp&>(original), basicAllocator)
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10, class A11, class A12, class A13,
          class A14, class A15, class A16, class A17>
template <class TYPE>
inline
Variant17<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
                A15, A16, A17>&
Variant17<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
                A15, A16, A17>
::operator=(const TYPE& value)
{
    Imp::operator=(value);
    return *this;
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10, class A11, class A12, class A13,
          class A14, class A15, class A16, class A17>
inline
Variant17<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
                A15, A16, A17>&
Variant17<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
                A15, A16, A17>::
operator=(const Variant17& rhs)
{
    Imp::operator=(static_cast<const Imp&>(rhs));
    return *this;
}
                       // ==========================
                       // class Variant18<...>
                       // ==========================

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17, class A18>
class Variant18 : public VariantImp<typename bslmf::TypeList18<
                                                    A1,  A2,  A3,  A4,  A5,
                                                    A6,  A7,  A8,  A9,  A10,
                                                    A11, A12, A13, A14, A15,
                                                    A16, A17, A18>::ListType> {
    // This class is a "specialization" of 'Variant' for a fixed number
    // (18) of types.  Its 18 template arguments *must* all be specified (none
    // are defaulted to 'bslmf::Nil').  It provides the same
    // functionality as 'Variant<A1, A2, ..., A18>'.

    // TYPES
    typedef VariantImp<typename bslmf::TypeList18<
                                                 A1,  A2,  A3,  A4,  A5,
                                                 A6,  A7,  A8,  A9,  A10,
                                                 A11, A12, A13, A14, A15,
                                                 A16, A17, A18>::ListType> Imp;

    typedef VariantImp_Traits<typename Imp::TypeList>             Traits;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant18,
                                      bslma::UsesBslmaAllocator,
                                      Traits::VARIANT_USES_BSLMA_ALLOCATOR);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant18,
                                      bsl::is_trivially_copyable,
                                      Traits::VARIANT_IS_BITWISE_COPYABLE);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant18,
                                      bslmf::IsBitwiseMoveable,
                                      Traits::VARIANT_IS_BITWISE_MOVEABLE);
    BSLMF_NESTED_TRAIT_DECLARATION(Variant18, bdlb::HasPrintMethod);


    // CREATORS
    Variant18();
        // Create a variant object.  The type and value of this object are
        // unset.  Use the currently installed default allocator to supply
        // memory.

    template <class TYPE_OR_ALLOCATOR>
    explicit Variant18(const TYPE_OR_ALLOCATOR& typeOrAllocator);
        // Create a variant object with the specified 'typeOrAlloc', which can
        // either be a type that the variant can hold, or an allocator to
        // supply memory.  If 'typeOrAlloc' is not a 'bslma::Allocator *', then
        // the variant will hold the value and type of 'typeOrAlloc', using
        // the currently installed default allocator to supply memory.
        // Otherwise, the variant will default construct the first specified
        // type in the type list using 'typeOrAlloc' to supply memory.  Note
        // that this templated constructor is created instead of creating an
        // second constructor that takes 'bslma::Allocator *' because templated
        // arguments are always a better match than child-to-base conversion.

    template <class TYPE>
    Variant18(const TYPE&       value,
                    bslma::Allocator *basicAllocator);
        // Create a variant object having the specified 'value' and that uses
        // the specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Variant18(const Variant18&  original,
                    bslma::Allocator       *basicAllocator = 0);
        // Create a variant object having the type and value of the specified
        // 'original' variant.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    // MANIPULATORS
    template <class TYPE>
    Variant18& operator=(const TYPE& value);
        // Assign to this object the specified 'value' of the specified 'TYPE',
        // and return a reference to this modifiable object.  The value
        // previously held by this variant (if any) will be destroyed if the
        // value's type is different from the parameterized 'TYPE'.

    Variant18& operator=(const Variant18& rhs);
        // Assign to this object the type and value currently hold by the
        // specified 'rhs' object, and return a reference to this modifiable
        // object.  The value previously held by this variant (if any) will be
        // destroyed if the value's type is different from the type held by the
        // 'rhs' object.
};

// CREATORS
template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10, class A11, class A12, class A13,
          class A14, class A15, class A16, class A17, class A18>
inline
Variant18<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
                A15, A16, A17, A18>::
Variant18()
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10, class A11, class A12, class A13,
          class A14, class A15, class A16, class A17, class A18>
template <class TYPE_OR_ALLOCATOR>
inline
Variant18<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
                A15, A16, A17, A18>::
Variant18(const TYPE_OR_ALLOCATOR& typeOrAlloc)
: Imp(typeOrAlloc)
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10, class A11, class A12, class A13,
          class A14, class A15, class A16, class A17, class A18>
template <class TYPE>
inline
Variant18<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
                A15, A16, A17, A18>::
Variant18(const TYPE&       value,
                bslma::Allocator *basicAllocator)
: Imp(value, basicAllocator)
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10, class A11, class A12, class A13,
          class A14, class A15, class A16, class A17, class A18>
inline
Variant18<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
                A15, A16, A17, A18>::
Variant18(const Variant18&  original,
                bslma::Allocator       *basicAllocator)
: Imp(static_cast<const Imp&>(original), basicAllocator)
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10, class A11, class A12, class A13,
          class A14, class A15, class A16, class A17, class A18>
template <class TYPE>
inline
Variant18<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
                A15, A16, A17, A18>&
Variant18<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
                A15, A16, A17, A18>
::operator=(const TYPE& value)
{
    Imp::operator=(value);
    return *this;
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10, class A11, class A12, class A13,
          class A14, class A15, class A16, class A17, class A18>
inline
Variant18<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
                A15, A16, A17, A18>&
Variant18<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
                A15, A16, A17, A18>::
operator=(const Variant18& rhs)
{
    Imp::operator=(static_cast<const Imp&>(rhs));
    return *this;
}

                       // ==========================
                       // class Variant19<...>
                       // ==========================

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17, class A18,
          class A19>
class Variant19 : public VariantImp<typename bslmf::TypeList19<
                                               A1,  A2,  A3,  A4,  A5,
                                               A6,  A7,  A8,  A9,  A10,
                                               A11, A12, A13, A14, A15,
                                               A16, A17, A18, A19>::ListType> {
    // This class is a "specialization" of 'Variant' for a fixed number
    // (19) of types.  Its 19 template arguments *must* all be specified (none
    // are defaulted to 'bslmf::Nil').  It provides the same
    // functionality as 'Variant<A1, A2, ..., A19>'.

    // TYPES
    typedef VariantImp<typename bslmf::TypeList19<
                                            A1,  A2,  A3,  A4,  A5,
                                            A6,  A7,  A8,  A9,  A10,
                                            A11, A12, A13, A14, A15,
                                            A16, A17, A18, A19>::ListType> Imp;

    typedef VariantImp_Traits<typename Imp::TypeList>             Traits;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant19,
                                      bslma::UsesBslmaAllocator,
                                      Traits::VARIANT_USES_BSLMA_ALLOCATOR);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant19,
                                      bsl::is_trivially_copyable,
                                      Traits::VARIANT_IS_BITWISE_COPYABLE);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant19,
                                      bslmf::IsBitwiseMoveable,
                                      Traits::VARIANT_IS_BITWISE_MOVEABLE);
    BSLMF_NESTED_TRAIT_DECLARATION(Variant19, bdlb::HasPrintMethod);

    // CREATORS
    Variant19();
        // Create a variant object.  The type and value of this object are
        // unset.  Use the currently installed default allocator to supply
        // memory.

    template <class TYPE_OR_ALLOCATOR>
    explicit Variant19(const TYPE_OR_ALLOCATOR& typeOrAllocator);
        // Create a variant object with the specified 'typeOrAlloc', which can
        // either be a type that the variant can hold, or an allocator to
        // supply memory.  If 'typeOrAlloc' is not a 'bslma::Allocator *', then
        // the variant will hold the value and type of 'typeOrAlloc', using
        // the currently installed default allocator to supply memory.
        // Otherwise, the variant will default construct the first specified
        // type in the type list using 'typeOrAlloc' to supply memory.  Note
        // that this templated constructor is created instead of creating an
        // second constructor that takes 'bslma::Allocator *' because templated
        // arguments are always a better match than child-to-base conversion.

    template <class TYPE>
    Variant19(const TYPE&       value,
                    bslma::Allocator *basicAllocator);
        // Create a variant object having the specified 'value' and that uses
        // the specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Variant19(const Variant19&  original,
                    bslma::Allocator       *basicAllocator = 0);
        // Create a variant object having the type and value of the specified
        // 'original' variant.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    // MANIPULATORS
    template <class TYPE>
    Variant19& operator=(const TYPE& value);
        // Assign to this object the specified 'value' of the specified 'TYPE',
        // and return a reference to this modifiable object.  The value
        // previously held by this variant (if any) will be destroyed if the
        // value's type is different from the parameterized 'TYPE'.

    Variant19& operator=(const Variant19& rhs);
        // Assign to this object the type and value currently hold by the
        // specified 'rhs' object, and return a reference to this modifiable
        // object.  The value previously held by this variant (if any) will be
        // destroyed if the value's type is different from the type held by the
        // 'rhs' object.
};

// CREATORS
template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10, class A11, class A12, class A13,
          class A14, class A15, class A16, class A17, class A18, class A19>
inline
Variant19<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
                A15, A16, A17, A18, A19>::
Variant19()
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10, class A11, class A12, class A13,
          class A14, class A15, class A16, class A17, class A18, class A19>
template <class TYPE_OR_ALLOCATOR>
inline
Variant19<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
                A15, A16, A17, A18, A19>::
Variant19(const TYPE_OR_ALLOCATOR& typeOrAlloc)
: Imp(typeOrAlloc)
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10, class A11, class A12, class A13,
          class A14, class A15, class A16, class A17, class A18, class A19>
template <class TYPE>
inline
Variant19<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
                A15, A16, A17, A18, A19>::
Variant19(const TYPE&       value,
                bslma::Allocator *basicAllocator)
: Imp(value, basicAllocator)
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10, class A11, class A12, class A13,
          class A14, class A15, class A16, class A17, class A18, class A19>
inline
Variant19<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
                A15, A16, A17, A18, A19>::
Variant19(const Variant19&  original,
                bslma::Allocator       *basicAllocator)
: Imp(static_cast<const Imp&>(original), basicAllocator)
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10, class A11, class A12, class A13,
          class A14, class A15, class A16, class A17, class A18, class A19>
template <class TYPE>
inline
Variant19<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
                A15, A16, A17, A18, A19>&
Variant19<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
                A15, A16, A17, A18, A19>
::operator=(const TYPE& value)
{
    Imp::operator=(value);
    return *this;
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10, class A11, class A12, class A13,
          class A14, class A15, class A16, class A17, class A18, class A19>
inline
Variant19<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
                A15, A16, A17, A18, A19>&
Variant19<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
                A15, A16, A17, A18, A19>::
operator=(const Variant19& rhs)
{
    Imp::operator=(static_cast<const Imp&>(rhs));
    return *this;
}

// ---- Anything below this line is implementation specific.  Do not use.  ----

               // ===========================================
               // struct Variant_TypeIndex<TYPES, TYPE>
               // ===========================================

template <typename TYPES, typename TYPE>
struct Variant_TypeIndex {
    // Component-private meta-function.  Do not use.  This meta-function
    // computed the index of the parameterized 'TYPE' in the parameterized list
    // of 'TYPES'.

    enum {
        k_VaL
            = bslmf::IsSame<
                      typename bslmf::TypeListTypeOf< 1, TYPES>::TypeOrDefault,
                      TYPE>::VALUE ?  1
            : bslmf::IsSame<
                      typename bslmf::TypeListTypeOf< 2, TYPES>::TypeOrDefault,
                      TYPE>::VALUE ?  2
            : bslmf::IsSame<
                      typename bslmf::TypeListTypeOf< 3, TYPES>::TypeOrDefault,
                      TYPE>::VALUE ?  3
            : bslmf::IsSame<
                      typename bslmf::TypeListTypeOf< 4, TYPES>::TypeOrDefault,
                      TYPE>::VALUE ?  4
            : bslmf::IsSame<
                      typename bslmf::TypeListTypeOf< 5, TYPES>::TypeOrDefault,
                      TYPE>::VALUE ?  5
            : bslmf::IsSame<
                      typename bslmf::TypeListTypeOf< 6, TYPES>::TypeOrDefault,
                      TYPE>::VALUE ?  6
            : bslmf::IsSame<
                      typename bslmf::TypeListTypeOf< 7, TYPES>::TypeOrDefault,
                      TYPE>::VALUE ?  7
            : bslmf::IsSame<
                      typename bslmf::TypeListTypeOf< 8, TYPES>::TypeOrDefault,
                      TYPE>::VALUE ?  8
            : bslmf::IsSame<
                      typename bslmf::TypeListTypeOf< 9, TYPES>::TypeOrDefault,
                      TYPE>::VALUE ?  9
            : bslmf::IsSame<
                      typename bslmf::TypeListTypeOf<10, TYPES>::TypeOrDefault,
                      TYPE>::VALUE ? 10
            : bslmf::IsSame<
                      typename bslmf::TypeListTypeOf<11, TYPES>::TypeOrDefault,
                      TYPE>::VALUE ? 11
            : bslmf::IsSame<
                      typename bslmf::TypeListTypeOf<12, TYPES>::TypeOrDefault,
                      TYPE>::VALUE ? 12
            : bslmf::IsSame<
                      typename bslmf::TypeListTypeOf<13, TYPES>::TypeOrDefault,
                      TYPE>::VALUE ? 13
            : bslmf::IsSame<
                      typename bslmf::TypeListTypeOf<14, TYPES>::TypeOrDefault,
                      TYPE>::VALUE ? 14
            : bslmf::IsSame<
                      typename bslmf::TypeListTypeOf<15, TYPES>::TypeOrDefault,
                      TYPE>::VALUE ? 15
            : bslmf::IsSame<
                      typename bslmf::TypeListTypeOf<16, TYPES>::TypeOrDefault,
                      TYPE>::VALUE ? 16
            : bslmf::IsSame<
                      typename bslmf::TypeListTypeOf<17, TYPES>::TypeOrDefault,
                      TYPE>::VALUE ? 17
            : bslmf::IsSame<
                      typename bslmf::TypeListTypeOf<18, TYPES>::TypeOrDefault,
                      TYPE>::VALUE ? 18
            : bslmf::IsSame<
                      typename bslmf::TypeListTypeOf<19, TYPES>::TypeOrDefault,
                      TYPE>::VALUE ? 19
            : bslmf::IsSame<
                      typename bslmf::TypeListTypeOf<20, TYPES>::TypeOrDefault,
                      TYPE>::VALUE ? 20
            : bslmf::IsConvertible<TYPE, bslma::Allocator *>::VALUE
                               ? 21
            : 0
    };

    BSLMF_ASSERT(k_VaL);
};

               // ============================================
               // struct Variant_DefaultConstructVisitor
               // ============================================

struct Variant_DefaultConstructVisitor {
    // This visitor, when invoked as a non-modifiable function object on an
    // initialized instance of some parameterized 'TYPE', will create a default
    // instance of that 'TYPE' in that location.

    // PUBLIC DATA
    bslma::Allocator *d_allocator_p;

    // CREATORS
    explicit Variant_DefaultConstructVisitor(bslma::Allocator *allocator)
    : d_allocator_p(allocator)
    {
    }

    // ACCESSORS
    template <class TYPE>
    inline
    void operator() (TYPE& value) const
    {
        bslalg::ScalarPrimitives::defaultConstruct(&value, d_allocator_p);
    }
};

                // =========================================
                // struct Variant_CopyConstructVisitor
                // =========================================

struct Variant_CopyConstructVisitor {
    // This visitor, when invoked as a non-modifiable function object on an
    // instance of some parameterized 'TYPE', will copy that instance to create
    // an instance of the same 'TYPE' in an uninitialized buffer specified at
    // construction of this visitor.

    // PUBLIC DATA
    void             *d_buffer_p;
    bslma::Allocator *d_allocator_p;

    // CREATORS
    Variant_CopyConstructVisitor(void             *buffer,
                                       bslma::Allocator *allocator)
    : d_buffer_p(buffer)
    , d_allocator_p(allocator)
    {
        BSLS_ASSERT_SAFE(d_buffer_p);
    }

    // ACCESSORS
    template <class TYPE>
    inline
    void operator() (const TYPE& value) const
    {
        bslalg::ScalarPrimitives::copyConstruct(
                                          reinterpret_cast<TYPE *>(d_buffer_p),
                                          value,
                                          d_allocator_p);
    }
};

                  // ======================================
                  // struct Variant_DestructorVisitor
                  // ======================================

struct Variant_DestructorVisitor {
    // This visitor, when invoked as a function object on an instance of
    // some parameterized 'TYPE', will destroy this instance.

    // ACCESSORS
    template <class TYPE>
    inline
    void operator() (TYPE& object) const
    {
        bslalg::ScalarDestructionPrimitives::destroy(&object);
    }
};

                    // ==================================
                    // struct Variant_AssignVisitor
                    // ==================================

struct Variant_AssignVisitor {
    // This visitor, when invoked as a non-modifiable function object on an
    // instance of some parameterized 'TYPE', will assign that instance to the
    // instance of the same 'TYPE' held in a buffer specified at construction
    // of this visitor.

    // PUBLIC DATA
    void *d_buffer_p;

    // CREATORS
    explicit Variant_AssignVisitor(void *buffer)
    : d_buffer_p(buffer)
    {
        BSLS_ASSERT_SAFE(d_buffer_p);
    }

    // ACCESSORS
    template <typename TYPE>
    inline
    void operator() (const TYPE& value)
    {
        *reinterpret_cast<TYPE *>(d_buffer_p) = value;
    }
};

                    // ================================
                    // struct Variant_SwapVisitor
                    // ================================

struct Variant_SwapVisitor {
    // This visitor swaps the variant object data that it holds with another
    // variant object data of parameterize 'TYPE'.  It requires that the two
    // variant objects being swapped contain data of the same type, and use the
    // same allocator.

    // PUBLIC DATA
    void *d_buffer_p;

    // CREATORS
    explicit Variant_SwapVisitor(void *buffer)
    : d_buffer_p(buffer)
    {
        BSLS_ASSERT_SAFE(d_buffer_p);
    }

    // MANIPULATORS
    template <typename TYPE>
    inline
    void operator() (TYPE& value)
    {
        bslalg::SwapUtil::swap(reinterpret_cast<TYPE *>(d_buffer_p), &value);
    }
};

           // ===================================================
           // struct Variant_MaxSupportedBdexVersionVisitor
           // ===================================================

struct Variant_MaxSupportedBdexVersionVisitor {
    // This visitor, when invoked as a non-modifiable function object on an
    // instance of some parameterized 'TYPE', will compute the maximal
    // supported 'bdex' version of the same 'TYPE' in its publically accessible
    // member 'd_maxSupportedBdexVersion'.

    // PUBLIC DATA
    int d_maxSupportedBdexVersion;

    // CREATORS
    Variant_MaxSupportedBdexVersionVisitor()
    : d_maxSupportedBdexVersion(bdex_VersionFunctions::BDEX_NO_VERSION_NUMBER)
    {
    }

    // ACCESSORS
    template <class TYPE>
    inline
    void operator() (const TYPE& object)
    {
        d_maxSupportedBdexVersion =
                            bdex_VersionFunctions::maxSupportedVersion(object);
    }

    inline
    void operator() (bslmf::Nil)
    {
        d_maxSupportedBdexVersion =
                                 bdex_VersionFunctions::BDEX_NO_VERSION_NUMBER;
    }
};

                 // ========================================
                 // struct Variant_BdexStreamInVisitor
                 // ========================================

template <class STREAM>
struct Variant_BdexStreamInVisitor {
    // This visitor, when invoked as a non-modifiable function object on an
    // initialized instance of some parameterized 'TYPE', will stream in
    // a value of the same 'TYPE' into that instance from a stream specified at
    // construction of this visitor,  using a version also specified at
    // construction of this visitor.

    // PUBLIC DATA
    STREAM& d_stream;   // held, not owned
    int     d_version;  // 'bdex' version

    // CREATORS
    Variant_BdexStreamInVisitor(STREAM& stream, int version)
    : d_stream(stream)
    , d_version(version)
    {
    }

    // ACCESSORS
    template <class VALUETYPE>
    inline
    void operator() (VALUETYPE& object) const
    {
        bdex_InStreamFunctions::streamIn(d_stream, object, d_version);
    }

    inline
    void operator() (bslmf::Nil) const
    {
        // no op
    }
};

                // =========================================
                // struct Variant_BdexStreamOutVisitor
                // =========================================

template <class STREAM>
struct Variant_BdexStreamOutVisitor {
    // This visitor, when invoked as a non-modifiable function object on an
    // initialized instance of some parameterized 'TYPE', will stream out the
    // value of that instance into a stream specified at construction of this
    // visitor, using a version also specified at construction of this
    // visitor.

    // PUBLIC DATA
    STREAM& d_stream;   // held, not owned
    int     d_version;  // 'bdex' version

    // CREATORS
    Variant_BdexStreamOutVisitor(STREAM& stream, int version)
    : d_stream(stream)
    , d_version(version)
    {
    }

    // ACCESSORS
    template <class VALUETYPE>
    inline
    void operator() (const VALUETYPE& object) const
    {
        bdex_OutStreamFunctions::streamOut(d_stream, object, d_version);
    }

    inline
    void operator() (bslmf::Nil) const
    {
        // no op
    }
};

                    // =================================
                    // struct Variant_PrintVisitor
                    // =================================

struct Variant_PrintVisitor {
    // This visitor, when invoked as a non-modifiable function object on an
    // instance of some parameterized 'TYPE', will print the value of that
    // instance into a stream specified at construction of this visitor,
    // using spacing information also specified at construction of this
    // visitor.

    // PUBLIC DATA
    bsl::ostream *d_stream_p;        // held, not owned
    int           d_level;
    int           d_spacesPerLevel;

    // CREATORS
    Variant_PrintVisitor(bsl::ostream *stream,
                               int           level,
                               int           spacesPerLevel)
    : d_stream_p(stream)
    , d_level(level)
    , d_spacesPerLevel(spacesPerLevel)
    {
        BSLS_ASSERT_SAFE(d_stream_p);
    }

    // ACCESSORS
    template <class TYPE>
    inline
    void operator() (const TYPE& value) const
    {
        bdlb::PrintMethods::print(*d_stream_p,
                                 value,
                                 d_level,
                                 d_spacesPerLevel);
    }

    inline
    void operator() (bslmf::Nil) const
    {
        // no op
    }
};

                 // ========================================
                 // struct Variant_EqualityTestVisitor
                 // ========================================

struct Variant_EqualityTestVisitor {
    // This visitor, when invoked as a non-modifiable function object on an
    // instance of some parameterized 'TYPE', will test the equality of the
    // value of that instance and of another instance held in a buffer
    // specified at construction of this visitor, and store the result into
    // its publically accessible 'd_result' member.

    // PUBLIC DATA
    mutable bool  d_result;
    const void   *d_buffer_p;  // held, not owned

    // CREATORS
    explicit Variant_EqualityTestVisitor(const void *buffer)
    : d_result(true)
    , d_buffer_p(buffer)
    {
        BSLS_ASSERT_SAFE(d_buffer_p);
    }

    // ACCESSORS
    template <class TYPE>
    inline
    void operator() (const TYPE& value) const
    {
        d_result = *reinterpret_cast<const TYPE *>(d_buffer_p) == value;
    }

    inline
    void operator() (bslmf::Nil) const
    {
        // 'value' and 'd_buffer_p' are guaranteed to be the same type by
        // 'operator=='.
        d_result = true;
    }
};

// ============================================================================
//                   TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ============================================================================

                   // -------------------------------------------
                   // class VariantImp_AllocatorBase<TYPES>
                   // -------------------------------------------

// CREATORS
template <typename TYPES>
inline
VariantImp_AllocatorBase<TYPES>::
VariantImp_AllocatorBase(int type, bslma::Allocator *basicAllocator)
: d_type(type)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

template <typename TYPES>
inline
VariantImp_AllocatorBase<TYPES>::
VariantImp_AllocatorBase(int,
                               bslma::Allocator  *basicAllocator,
                               bslmf::MetaInt<1> *)
: d_type(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

template <typename TYPES>
template <typename TYPE>
inline
VariantImp_AllocatorBase<TYPES>::
VariantImp_AllocatorBase(int type, const TYPE&, bslmf::MetaInt<0> *)
: d_type(type)
, d_allocator_p(bslma::Default::allocator(0))
{
}

// ACCESSORS
template <typename TYPES>
inline
bslma::Allocator *
VariantImp_AllocatorBase<TYPES>::getAllocator() const
{
    return d_allocator_p;
}

                  // ----------------------------------------------
                  // class VariantImp_NonAllocatorBase<TYPES>
                  // ----------------------------------------------

// CREATORS
template <typename TYPES>
inline
VariantImp_NonAllocatorBase<TYPES>::
VariantImp_NonAllocatorBase(int type, bslma::Allocator *)
: d_type(type)
{
}

template <typename TYPES>
inline
VariantImp_NonAllocatorBase<TYPES>::
VariantImp_NonAllocatorBase(int, bslma::Allocator *, bslmf::MetaInt<1> *)
: d_type(0)
{
}

template <typename TYPES>
template <typename TYPE>
inline
VariantImp_NonAllocatorBase<TYPES>::
VariantImp_NonAllocatorBase(int type, const TYPE&, bslmf::MetaInt<0> *)
: d_type(type)
{
}

// ACCESSORS
template <typename TYPES>
inline
bslma::Allocator *
VariantImp_NonAllocatorBase<TYPES>::getAllocator() const
{
    return 0;
}

                       // ---------------------------------
                       // class Variant_ReturnAnyType
                       // ---------------------------------

inline
void Variant_ReturnAnyTypeUtil::garbage(void *)
{
}

template <class TYPE>
inline
TYPE Variant_ReturnAnyTypeUtil::garbage(TYPE *dummy)
{
    return *dummy;
}
}  // close package namespace

template <class TYPE>
inline
TYPE bdlb::Variant_ReturnAnyTypeUtil::garbage()
{
    // Note that IBM xlc requires we explicitly declare a temporary here,
    // rather than cast the null pointer directly as a function argument.

    TYPE *const ptr = 0;
    return garbage(ptr);
};

namespace bdlb {
                       // ------------------------------------
                       // class Variant_RawVisitorHelper
                       // ------------------------------------


template <class RESULT_TYPE, class VISITOR>
inline
Variant_RawVisitorHelper<RESULT_TYPE, VISITOR>::
Variant_RawVisitorHelper(VISITOR *visitor)
: d_visitor(visitor)
{
    BSLS_ASSERT(0 != visitor);
}

template <class RESULT_TYPE, class VISITOR>
inline
RESULT_TYPE
Variant_RawVisitorHelper<RESULT_TYPE, VISITOR>::operator()(
                                                              bslmf::Nil) const
{
    BSLS_ASSERT_OPT(false);
    return Variant_ReturnAnyTypeUtil::garbage<RESULT_TYPE>();
}

template <class RESULT_TYPE, class VISITOR>
template <class ARGUMENT_TYPE>
inline
RESULT_TYPE
Variant_RawVisitorHelper<RESULT_TYPE, VISITOR>::operator()(
                                                       ARGUMENT_TYPE& argument)
{
    return static_cast<RESULT_TYPE>((*d_visitor)(argument));
}

template <class RESULT_TYPE, class VISITOR>
template <class ARGUMENT_TYPE>
inline
RESULT_TYPE
Variant_RawVisitorHelper<RESULT_TYPE, VISITOR>::operator()(
                                                 const ARGUMENT_TYPE& argument)
{
    return static_cast<RESULT_TYPE>((*d_visitor)(argument));
}

template <class RESULT_TYPE, class VISITOR>
template <class ARGUMENT_TYPE>
inline
RESULT_TYPE
Variant_RawVisitorHelper<RESULT_TYPE, VISITOR>::operator()(
                                                 ARGUMENT_TYPE& argument) const
{
    return static_cast<RESULT_TYPE>((*d_visitor)(argument));
}

template <class RESULT_TYPE, class VISITOR>
template <class ARGUMENT_TYPE>
inline
RESULT_TYPE
Variant_RawVisitorHelper<RESULT_TYPE, VISITOR>::operator()(
                                           const ARGUMENT_TYPE& argument) const
{
    return static_cast<RESULT_TYPE>((*d_visitor)(argument));
}

                        // -----------------------------
                        // class VariantImp<TYPES>
                        // -----------------------------

// PRIVATE MANIPULATORS
template <class TYPES>
template <class TYPE, class VISITOR_REF>
inline
void VariantImp<TYPES>::applyImp(VISITOR_REF        visitor,
                                       bslmf::MetaInt<0> *)
{
    visitor(reinterpret_cast<
                        bsls::ObjectBuffer<TYPE> *>(&this->d_value)->object());
}

template <class TYPES>
template <class TYPE, class VISITOR_REF>
inline
void VariantImp<TYPES>::applyImp(VISITOR_REF        visitor,
                                       bslmf::MetaInt<1> *)
{
    bslmf::Nil nil = bslmf::Nil();
    visitor(nil);
}

template <class TYPES>
template <class TYPE, class VISITOR_REF>
inline
void VariantImp<TYPES>::applyImp(VISITOR_REF visitor)
{
    typedef typename bslmf::IsSame<TYPE, bslmf::Nil>::Type IsUnset;
    applyImp<TYPE, VISITOR_REF>(visitor, (IsUnset *)0);
}

template <class TYPES>
template <class TYPE, class VISITOR_REF, class RET_TYPE>
inline
RET_TYPE VariantImp<TYPES>::applyImpR(VISITOR_REF        visitor,
                                            bslmf::MetaInt<0> *)
{
    return visitor(reinterpret_cast<
                        bsls::ObjectBuffer<TYPE> *>(&this->d_value)->object());
}

template <class TYPES>
template <class TYPE, class VISITOR_REF, class RET_TYPE>
inline
RET_TYPE VariantImp<TYPES>::applyImpR(VISITOR_REF        visitor,
                                            bslmf::MetaInt<1> *)
{
    bslmf::Nil nil = bslmf::Nil();
    return visitor(nil);
}

template <class TYPES>
template <class TYPE, class VISITOR_REF, class RET_TYPE>
inline
RET_TYPE VariantImp<TYPES>::applyImpR(VISITOR_REF visitor)
{
    typedef typename bslmf::IsSame<TYPE, bslmf::Nil>::Type IsUnset;
    return applyImpR<TYPE, VISITOR_REF, RET_TYPE>(visitor, (IsUnset *)0);
}

template <class TYPES>
template <class TYPE, class SOURCE_TYPE>
void VariantImp<TYPES>::assignImp(const SOURCE_TYPE& value)
{
    reset();
    bslalg::ScalarPrimitives::construct(
     &(reinterpret_cast<bsls::ObjectBuffer<TYPE> *>(&this->d_value)->object()),
     value,
     this->getAllocator());

    this->d_type = Variant_TypeIndex<TYPES, TYPE>::k_VaL;
}

template <class TYPES>
template <class VISITOR_REF>
void VariantImp<TYPES>::doApply(VISITOR_REF visitor, int type)
{
    switch (type) {
      case 0: {
        BSLS_ASSERT(!"'applyRaw' invoked on an unset variant");
      } break;
      case 1: {
        applyImp<typename Base::Type1, VISITOR_REF>(visitor);
      } break;
      case 2: {
        applyImp<typename Base::Type2, VISITOR_REF>(visitor);
      } break;
      case 3: {
        applyImp<typename Base::Type3, VISITOR_REF>(visitor);
      } break;
      case 4: {
        applyImp<typename Base::Type4, VISITOR_REF>(visitor);
      } break;
      case 5: {
        applyImp<typename Base::Type5, VISITOR_REF>(visitor);
      } break;
      case 6: {
        applyImp<typename Base::Type6, VISITOR_REF>(visitor);
      } break;
      case 7: {
        applyImp<typename Base::Type7, VISITOR_REF>(visitor);
      } break;
      case 8: {
        applyImp<typename Base::Type8, VISITOR_REF>(visitor);
      } break;
      case 9: {
        applyImp<typename Base::Type9, VISITOR_REF>(visitor);
      } break;
      case 10: {
        applyImp<typename Base::Type10, VISITOR_REF>(visitor);
      } break;
      case 11: {
        applyImp<typename Base::Type11, VISITOR_REF>(visitor);
      } break;
      case 12: {
        applyImp<typename Base::Type12, VISITOR_REF>(visitor);
      } break;
      case 13: {
        applyImp<typename Base::Type13, VISITOR_REF>(visitor);
      } break;
      case 14: {
        applyImp<typename Base::Type14, VISITOR_REF>(visitor);
      } break;
      case 15: {
        applyImp<typename Base::Type15, VISITOR_REF>(visitor);
      } break;
      case 16: {
        applyImp<typename Base::Type16, VISITOR_REF>(visitor);
      } break;
      case 17: {
        applyImp<typename Base::Type17, VISITOR_REF>(visitor);
      } break;
      case 18: {
        applyImp<typename Base::Type18, VISITOR_REF>(visitor);
      } break;
      case 19: {
        applyImp<typename Base::Type19, VISITOR_REF>(visitor);
      } break;
      case 20: {
        applyImp<typename Base::Type20, VISITOR_REF>(visitor);
      } break;
      default: {
        BSLS_ASSERT_SAFE(!"Unreachable by design!");
      } break;
    }
}

template <class TYPES>
template <class TYPE>
inline
void VariantImp<TYPES>::create(const TYPE& value, bslmf::MetaInt<0> *)
{
    bslalg::ScalarPrimitives::construct(
        &(reinterpret_cast<bsls::ObjectBuffer<TYPE> *>(
                                                    &this->d_value)->object()),
        value,
        this->getAllocator());

}

template <class TYPES>
inline
void VariantImp<TYPES>::create(bslma::Allocator *, bslmf::MetaInt<1> *)
{
}

template <class TYPES>
template <class VISITOR_REF, class RET_TYPE>
RET_TYPE VariantImp<TYPES>::doApplyR(VISITOR_REF visitor,
                                           int         type)
{
    switch (type) {
      case 0: {
        BSLS_ASSERT(!"'applyRaw' invoked on an unset variant");
      } break;
      case 1: {
        return applyImpR<typename Base::Type1,
                                         VISITOR_REF, RET_TYPE>(visitor);
      } break;
      case 2: {
        return applyImpR<typename Base::Type2,
                                         VISITOR_REF, RET_TYPE>(visitor);
      } break;
      case 3: {
        return applyImpR<typename Base::Type3,
                                         VISITOR_REF, RET_TYPE>(visitor);
      } break;
      case 4: {
        return applyImpR<typename Base::Type4,
                                         VISITOR_REF, RET_TYPE>(visitor);
      } break;
      case 5: {
        return applyImpR<typename Base::Type5,
                                         VISITOR_REF, RET_TYPE>(visitor);
      } break;
      case 6: {
        return applyImpR<typename Base::Type6,
                                         VISITOR_REF, RET_TYPE>(visitor);
      } break;
      case 7: {
        return applyImpR<typename Base::Type7,
                                         VISITOR_REF, RET_TYPE>(visitor);
      } break;
      case 8: {
        return applyImpR<typename Base::Type8,
                                         VISITOR_REF, RET_TYPE>(visitor);
      } break;
      case 9: {
        return applyImpR<typename Base::Type9,
                                         VISITOR_REF, RET_TYPE>(visitor);
      } break;
      case 10: {
        return applyImpR<typename Base::Type10,
                                         VISITOR_REF, RET_TYPE>(visitor);
      } break;
      case 11: {
        return applyImpR<typename Base::Type11,
                                         VISITOR_REF, RET_TYPE>(visitor);
      } break;
      case 12: {
        return applyImpR<typename Base::Type12,
                                         VISITOR_REF, RET_TYPE>(visitor);
      } break;
      case 13: {
        return applyImpR<typename Base::Type13,
                                         VISITOR_REF, RET_TYPE>(visitor);
      } break;
      case 14: {
        return applyImpR<typename Base::Type14,
                                         VISITOR_REF, RET_TYPE>(visitor);
      } break;
      case 15: {
        return applyImpR<typename Base::Type15,
                                         VISITOR_REF, RET_TYPE>(visitor);
      } break;
      case 16: {
        return applyImpR<typename Base::Type16,
                                         VISITOR_REF, RET_TYPE>(visitor);
      } break;
      case 17: {
        return applyImpR<typename Base::Type17,
                                         VISITOR_REF, RET_TYPE>(visitor);
      } break;
      case 18: {
        return applyImpR<typename Base::Type18,
                                         VISITOR_REF, RET_TYPE>(visitor);
      } break;
      case 19: {
        return applyImpR<typename Base::Type19,
                                         VISITOR_REF, RET_TYPE>(visitor);
      } break;
      case 20: {
        return applyImpR<typename Base::Type20,
                                         VISITOR_REF, RET_TYPE>(visitor);
      } break;
      default: {
        BSLS_ASSERT_SAFE(!"Unreachable by design!");
      } break;
    }

    // Unreachable by design, just here to quiet down compiler.
    return Variant_ReturnAnyTypeUtil::garbage<RET_TYPE>();
}

// PRIVATE ACCESSORS
template <class TYPES>
template <class TYPE, class VISITOR_REF>
inline
void VariantImp<TYPES>::applyImp(VISITOR_REF        visitor,
                                       bslmf::MetaInt<0> *) const
{
    visitor(reinterpret_cast<
                  const bsls::ObjectBuffer<TYPE> *>(&this->d_value)->object());
}

template <class TYPES>
template <class TYPE, class VISITOR_REF>
inline
void VariantImp<TYPES>::applyImp(VISITOR_REF        visitor,
                                       bslmf::MetaInt<1> *) const
{
    bslmf::Nil nil = bslmf::Nil();
    visitor(nil);
}

template <class TYPES>
template <class TYPE, class VISITOR_REF>
inline
void VariantImp<TYPES>::applyImp(VISITOR_REF visitor) const
{
    typedef typename bslmf::IsSame<TYPE, bslmf::Nil>::Type IsUnset;
    applyImp<TYPE, VISITOR_REF>(visitor, (IsUnset *)0);
}

template <class TYPES>
template <class TYPE, class VISITOR_REF, class RET_TYPE>
inline
RET_TYPE VariantImp<TYPES>::applyImpR(VISITOR_REF        visitor,
                                            bslmf::MetaInt<0> *) const
{
    return visitor(reinterpret_cast<
                  const bsls::ObjectBuffer<TYPE> *>(&this->d_value)->object());
}

template <class TYPES>
template <class TYPE, class VISITOR_REF, class RET_TYPE>
inline
RET_TYPE VariantImp<TYPES>::applyImpR(VISITOR_REF        visitor,
                                            bslmf::MetaInt<1> *) const
{
    bslmf::Nil nil = bslmf::Nil();
    return visitor(nil);
}

template <class TYPES>
template <class TYPE, class VISITOR_REF, class RET_TYPE>
inline
RET_TYPE VariantImp<TYPES>::applyImpR(VISITOR_REF visitor) const
{
    typedef typename bslmf::IsSame<TYPE, bslmf::Nil>::Type IsUnset;
    return applyImpR<TYPE, VISITOR_REF, RET_TYPE>(visitor, (IsUnset *)0);
}

template <class TYPES>
template <class VISITOR_REF>
void VariantImp<TYPES>::doApply(VISITOR_REF visitor,
                                      int         type) const
{
    switch (type) {
      case 0: {
        BSLS_ASSERT(!"'applyRaw' invoked on an unset variant");
      } break;
      case 1: {
        applyImp<typename Base::Type1, VISITOR_REF>(visitor);
      } break;
      case 2: {
        applyImp<typename Base::Type2, VISITOR_REF>(visitor);
      } break;
      case 3: {
        applyImp<typename Base::Type3, VISITOR_REF>(visitor);
      } break;
      case 4: {
        applyImp<typename Base::Type4, VISITOR_REF>(visitor);
      } break;
      case 5: {
        applyImp<typename Base::Type5, VISITOR_REF>(visitor);
      } break;
      case 6: {
        applyImp<typename Base::Type6, VISITOR_REF>(visitor);
      } break;
      case 7: {
        applyImp<typename Base::Type7, VISITOR_REF>(visitor);
      } break;
      case 8: {
        applyImp<typename Base::Type8, VISITOR_REF>(visitor);
      } break;
      case 9: {
        applyImp<typename Base::Type9, VISITOR_REF>(visitor);
      } break;
      case 10: {
        applyImp<typename Base::Type10, VISITOR_REF>(visitor);
      } break;
      case 11: {
        applyImp<typename Base::Type11, VISITOR_REF>(visitor);
      } break;
      case 12: {
        applyImp<typename Base::Type12, VISITOR_REF>(visitor);
      } break;
      case 13: {
        applyImp<typename Base::Type13, VISITOR_REF>(visitor);
      } break;
      case 14: {
        applyImp<typename Base::Type14, VISITOR_REF>(visitor);
      } break;
      case 15: {
        applyImp<typename Base::Type15, VISITOR_REF>(visitor);
      } break;
      case 16: {
        applyImp<typename Base::Type16, VISITOR_REF>(visitor);
      } break;
      case 17: {
        applyImp<typename Base::Type17, VISITOR_REF>(visitor);
      } break;
      case 18: {
        applyImp<typename Base::Type18, VISITOR_REF>(visitor);
      } break;
      case 19: {
        applyImp<typename Base::Type19, VISITOR_REF>(visitor);
      } break;
      case 20: {
        applyImp<typename Base::Type20, VISITOR_REF>(visitor);
      } break;
      default: {
        BSLS_ASSERT_SAFE(!"Unreachable by design!");
      } break;
    }
}

template <class TYPES>
template <class VISITOR_REF, class RET_TYPE>
RET_TYPE VariantImp<TYPES>::doApplyR(VISITOR_REF visitor,
                                           int         type) const
{
    switch (type) {
      case 0: {
        BSLS_ASSERT(!"'applyRaw' invoked on an unset variant");
      } break;
      case 1: {
        return applyImpR<typename Base::Type1,
                                         VISITOR_REF, RET_TYPE>(visitor);
      } break;
      case 2: {
        return applyImpR<typename Base::Type2,
                                         VISITOR_REF, RET_TYPE>(visitor);
      } break;
      case 3: {
        return applyImpR<typename Base::Type3,
                                         VISITOR_REF, RET_TYPE>(visitor);
      } break;
      case 4: {
        return applyImpR<typename Base::Type4,
                                         VISITOR_REF, RET_TYPE>(visitor);
      } break;
      case 5: {
        return applyImpR<typename Base::Type5,
                                         VISITOR_REF, RET_TYPE>(visitor);
      } break;
      case 6: {
        return applyImpR<typename Base::Type6,
                                         VISITOR_REF, RET_TYPE>(visitor);
      } break;
      case 7: {
        return applyImpR<typename Base::Type7,
                                         VISITOR_REF, RET_TYPE>(visitor);
      } break;
      case 8: {
        return applyImpR<typename Base::Type8,
                                         VISITOR_REF, RET_TYPE>(visitor);
      } break;
      case 9: {
        return applyImpR<typename Base::Type9,
                                         VISITOR_REF, RET_TYPE>(visitor);
      } break;
      case 10: {
        return applyImpR<typename Base::Type10,
                                         VISITOR_REF, RET_TYPE>(visitor);
      } break;
      case 11: {
        return applyImpR<typename Base::Type11,
                                         VISITOR_REF, RET_TYPE>(visitor);
      } break;
      case 12: {
        return applyImpR<typename Base::Type12,
                                         VISITOR_REF, RET_TYPE>(visitor);
      } break;
      case 13: {
        return applyImpR<typename Base::Type13,
                                         VISITOR_REF, RET_TYPE>(visitor);
      } break;
      case 14: {
        return applyImpR<typename Base::Type14,
                                         VISITOR_REF, RET_TYPE>(visitor);
      } break;
      case 15: {
        return applyImpR<typename Base::Type15,
                                         VISITOR_REF, RET_TYPE>(visitor);
      } break;
      case 16: {
        return applyImpR<typename Base::Type16,
                                         VISITOR_REF, RET_TYPE>(visitor);
      } break;
      case 17: {
        return applyImpR<typename Base::Type17,
                                         VISITOR_REF, RET_TYPE>(visitor);
      } break;
      case 18: {
        return applyImpR<typename Base::Type18,
                                         VISITOR_REF, RET_TYPE>(visitor);
      } break;
      case 19: {
        return applyImpR<typename Base::Type19,
                                         VISITOR_REF, RET_TYPE>(visitor);
      } break;
      case 20: {
        return applyImpR<typename Base::Type20,
                                         VISITOR_REF, RET_TYPE>(visitor);
      } break;
      default: {
        BSLS_ASSERT_SAFE(!"Unreachable by design!");
      } break;
    }

    // Unreachable by design, just here to quiet down compiler
    return Variant_ReturnAnyTypeUtil::garbage<RET_TYPE>();
}

// CREATORS
template <class TYPES>
inline
VariantImp<TYPES>::VariantImp()
: Base(0, 0)
{
}

template <class TYPES>
template <class TYPE_OR_ALLOCATOR>
inline
VariantImp<TYPES>::VariantImp(const TYPE_OR_ALLOCATOR& typeOrAlloc)
: Base(Variant_TypeIndex<TYPES, TYPE_OR_ALLOCATOR>::k_VaL,
       typeOrAlloc,
       (bslmf::MetaInt< bslmf::IsConvertible<TYPE_OR_ALLOCATOR,
                                           bslma::Allocator *>::VALUE> *)0)
{
    enum {
        k_IS_ALLOCATOR = bslmf::IsConvertible<TYPE_OR_ALLOCATOR,
                                              bslma::Allocator *>::VALUE
    };

    create(typeOrAlloc, (bslmf::MetaInt<k_IS_ALLOCATOR> *)0);
}

template <class TYPES>
template <class TYPE>
inline
VariantImp<TYPES>::VariantImp(const TYPE&       value,
                                          bslma::Allocator *basicAllocator)
: Base(Variant_TypeIndex<TYPES, TYPE>::k_VaL, basicAllocator)
{
    bslalg::ScalarPrimitives::construct(
     &(reinterpret_cast<bsls::ObjectBuffer<TYPE> *>(&this->d_value)->object()),
     value,
     this->getAllocator());
}

template <class TYPES>
VariantImp<TYPES>::VariantImp(
                                      const VariantImp&  original,
                                      bslma::Allocator        *basicAllocator)
: Base(original.d_type, basicAllocator)
{
    if (this->d_type) {
        Variant_CopyConstructVisitor visitor(&this->d_value,
                                                   this->getAllocator());
        original.apply(visitor);
    }
}

template <class TYPES>
inline
VariantImp<TYPES>::~VariantImp()
{
    reset();
}

// MANIPULATORS
template <class TYPES>
template <class TYPE>
inline
VariantImp<TYPES>&
VariantImp<TYPES>::operator=(const TYPE& value)
{
    return assign(value);
}

template <class TYPES>
VariantImp<TYPES>&
VariantImp<TYPES>::operator=(const VariantImp& rhs)
{
    if (&rhs != this) {
        if (this->d_type == rhs.d_type) {
            if (this->d_type) {
                Variant_AssignVisitor assigner(&this->d_value);
                rhs.apply(assigner);
            }
        }
        else {
            reset();
            if (rhs.d_type) {
                Variant_CopyConstructVisitor copyConstructor(
                                                         &this->d_value,
                                                         this->getAllocator());
                rhs.apply(copyConstructor);
                this->d_type = rhs.d_type;
            }
        }
    }

    return *this;
}

template <class TYPES>
template <class RET_TYPE, class VISITOR>
inline
RET_TYPE VariantImp<TYPES>::apply(VISITOR& visitor)
{
    if (this->d_type) {
        return doApplyR<VISITOR&, RET_TYPE>(visitor, this->d_type);
    }

    bslmf::Nil nil = bslmf::Nil();
    return visitor(nil);
}

template <class TYPES>
template <class RET_TYPE, class VISITOR>
inline
RET_TYPE VariantImp<TYPES>::apply(const VISITOR& visitor)
{
    if (this->d_type) {
        return doApplyR<const VISITOR&, RET_TYPE>(visitor, this->d_type);
    }

    bslmf::Nil nil = bslmf::Nil();
    return visitor(nil);
}

template <class TYPES>
template <class RET_TYPE, class VISITOR, class TYPE>
inline
RET_TYPE VariantImp<TYPES>::apply(VISITOR&    visitor,
                                        const TYPE& defaultValue)
{
    if (this->d_type) {
        return doApplyR<VISITOR&, RET_TYPE>(visitor, this->d_type);
    }

    return visitor(defaultValue);
}

template <class TYPES>
template <class RET_TYPE, class VISITOR, class TYPE>
inline
RET_TYPE VariantImp<TYPES>::apply(const VISITOR& visitor,
                                        const TYPE&    defaultValue)
{
    if (this->d_type) {
        return doApplyR<const VISITOR&, RET_TYPE>(visitor, this->d_type);
    }

    return visitor(defaultValue);
}

template <class TYPES>
template <class RET_TYPE, class VISITOR>
inline
RET_TYPE VariantImp<TYPES>::applyRaw(VISITOR& visitor)
{

    typedef Variant_RawVisitorHelper<
                               RET_TYPE,
                               VISITOR> Helper;

    return doApplyR<const Helper&, RET_TYPE>(Helper(visitor),
                                             this->d_type);

}

template <class TYPES>
template <class RET_TYPE, class VISITOR>
inline
RET_TYPE VariantImp<TYPES>::applyRaw(const VISITOR& visitor)
{

    typedef Variant_RawVisitorHelper<
                               RET_TYPE,
                               const VISITOR> Helper;

    return doApplyR<const Helper&, RET_TYPE>(Helper(visitor),
                                             this->d_type);
}

template <class TYPES>
template <class TYPE>
VariantImp<TYPES>& VariantImp<TYPES>::assign(const TYPE& value)
{
    if (Variant_TypeIndex<TYPES, TYPE>::k_VaL == this->d_type) {
        reinterpret_cast<bsls::ObjectBuffer<TYPE> *>(&this->d_value)->object()
                                                                       = value;
    }
    else {
        assignImp<TYPE, TYPE>(value);
    }

    return *this;
}

template <class TYPES>
template <class TYPE, class SOURCE_TYPE>
VariantImp<TYPES>& VariantImp<TYPES>::assignTo(
                                                      const SOURCE_TYPE& value)
{
    if (Variant_TypeIndex<TYPES, TYPE>::k_VaL == this->d_type
     && bslmf::IsSame<TYPE, SOURCE_TYPE>::VALUE) {
        reinterpret_cast<bsls::ObjectBuffer<TYPE> *>(
                                             &this->d_value)->object() = value;
    }
    else {
        assignImp<TYPE, SOURCE_TYPE>(value);
    }

    return *this;
}

template <class TYPES>
template <class STREAM>
STREAM& VariantImp<TYPES>::bdexStreamIn(STREAM& stream, int version)
{
    int type;
    bdex_InStreamFunctions::streamIn(stream, type, 0);

    if (!stream || type < 0 || 20 < type) {
        stream.invalidate();
        return stream;
    }

    if (type != this->d_type) {
        reset();

        if (type) {
            Variant_DefaultConstructVisitor defaultConstructor(
                                                         this->getAllocator());
            doApply(defaultConstructor, type);
        }

        this->d_type = type;
    }

    if (type) {
        Variant_BdexStreamInVisitor<STREAM> streamer(stream, version);
        doApply(streamer, type);
    }

    return stream;
}

template <class TYPES>
template <class TYPE>
inline
void VariantImp<TYPES>::createInPlace()
{
    typedef bsls::ObjectBuffer<TYPE> BufferType;

    reset();
    bslalg::ScalarPrimitives::defaultConstruct(
                   &(reinterpret_cast<BufferType *>(&this->d_value)->object()),
                   this->getAllocator());
    this->d_type = Variant_TypeIndex<TYPES, TYPE>::k_VaL;
}

template <class TYPES>
template <class TYPE, class A1>
inline
void VariantImp<TYPES>::createInPlace(const A1& a1)
{
    typedef bsls::ObjectBuffer<TYPE> BufferType;

    reset();
    bslalg::ScalarPrimitives::construct(
                   &(reinterpret_cast<BufferType *>(&this->d_value)->object()),
                   a1, this->getAllocator());
    this->d_type = Variant_TypeIndex<TYPES, TYPE>::k_VaL;
}

template <class TYPES>
template <class TYPE, class A1, class A2>
inline
void VariantImp<TYPES>::createInPlace(const A1& a1, const A2& a2)
{
    typedef bsls::ObjectBuffer<TYPE> BufferType;

    reset();
    bslalg::ScalarPrimitives::construct(
                   &(reinterpret_cast<BufferType *>(&this->d_value)->object()),
                   a1, a2, this->getAllocator());
    this->d_type = Variant_TypeIndex<TYPES, TYPE>::k_VaL;
}

template <class TYPES>
template <class TYPE, class A1, class A2, class A3>
inline
void VariantImp<TYPES>::createInPlace(
                                      const A1& a1, const A2& a2, const A3& a3)
{
    typedef bsls::ObjectBuffer<TYPE> BufferType;

    reset();
    bslalg::ScalarPrimitives::construct(
                   &(reinterpret_cast<BufferType *>(&this->d_value)->object()),
                   a1, a2, a3, this->getAllocator());
    this->d_type = Variant_TypeIndex<TYPES, TYPE>::k_VaL;
}

template <class TYPES>
template <class TYPE, class A1, class A2, class A3, class A4>
inline
void VariantImp<TYPES>::createInPlace(
                        const A1& a1, const A2& a2, const A3& a3, const A4& a4)
{
    typedef bsls::ObjectBuffer<TYPE> BufferType;

    reset();
    bslalg::ScalarPrimitives::construct(
                   &(reinterpret_cast<BufferType *>(&this->d_value)->object()),
                   a1, a2, a3, a4, this->getAllocator());
    this->d_type = Variant_TypeIndex<TYPES, TYPE>::k_VaL;
}

template <class TYPES>
template <class TYPE, class A1, class A2, class A3, class A4, class A5>
inline
void VariantImp<TYPES>::createInPlace(
                        const A1& a1, const A2& a2, const A3& a3, const A4& a4,
                        const A5& a5)
{
    typedef bsls::ObjectBuffer<TYPE> BufferType;

    reset();
    bslalg::ScalarPrimitives::construct(
                  &(reinterpret_cast<BufferType *>(&this->d_value)->object()),
                  a1, a2, a3, a4, a5, this->getAllocator());
    this->d_type = Variant_TypeIndex<TYPES, TYPE>::k_VaL;
}

template <class TYPES>
template <class TYPE, class A1, class A2, class A3, class A4, class A5,
                      class A6>
inline
void VariantImp<TYPES>::createInPlace(
                        const A1& a1, const A2& a2, const A3& a3, const A4& a4,
                        const A5& a5, const A6& a6)
{
    typedef bsls::ObjectBuffer<TYPE> BufferType;

    reset();
    bslalg::ScalarPrimitives::construct(
                  &(reinterpret_cast<BufferType *>(&this->d_value)->object()),
                  a1, a2, a3, a4, a5, a6, this->getAllocator());
    this->d_type = Variant_TypeIndex<TYPES, TYPE>::k_VaL;
}

template <class TYPES>
template <class TYPE, class A1, class A2, class A3, class A4, class A5,
                      class A6, class A7>
inline
void VariantImp<TYPES>::createInPlace(
                        const A1& a1, const A2& a2, const A3& a3, const A4& a4,
                        const A5& a5, const A6& a6, const A7& a7)
{
    typedef bsls::ObjectBuffer<TYPE> BufferType;

    reset();
    bslalg::ScalarPrimitives::construct(
                  &(reinterpret_cast<BufferType *>(&this->d_value)->object()),
                  a1, a2, a3, a4, a5, a6, a7, this->getAllocator());
    this->d_type = Variant_TypeIndex<TYPES, TYPE>::k_VaL;
}

template <class TYPES>
template <class TYPE, class A1, class A2, class A3, class A4, class A5,
                      class A6, class A7, class A8>
inline
void VariantImp<TYPES>::createInPlace(
                        const A1& a1, const A2& a2, const A3& a3, const A4& a4,
                        const A5& a5, const A6& a6, const A7& a7, const A8& a8)
{
    typedef bsls::ObjectBuffer<TYPE> BufferType;

    reset();
    bslalg::ScalarPrimitives::construct(
                  &(reinterpret_cast<BufferType *>(&this->d_value)->object()),
                  a1, a2, a3, a4, a5, a6, a7, a8, this->getAllocator());
    this->d_type = Variant_TypeIndex<TYPES, TYPE>::k_VaL;
}

template <class TYPES>
template <class TYPE, class A1, class A2, class A3, class A4, class A5,
                      class A6, class A7, class A8, class A9>
inline
void VariantImp<TYPES>::createInPlace(
                        const A1& a1, const A2& a2, const A3& a3, const A4& a4,
                        const A5& a5, const A6& a6, const A7& a7, const A8& a8,
                        const A9& a9)
{
    typedef bsls::ObjectBuffer<TYPE> BufferType;

    reset();
    bslalg::ScalarPrimitives::construct(
                  &(reinterpret_cast<BufferType *>(&this->d_value)->object()),
                  a1, a2, a3, a4, a5, a6, a7, a8, a9,
                  this->getAllocator());
    this->d_type = Variant_TypeIndex<TYPES, TYPE>::k_VaL;
}

template <class TYPES>
template <class TYPE, class A1, class A2, class A3, class A4, class A5,
                      class A6, class A7, class A8, class A9, class A10>
inline
void VariantImp<TYPES>::createInPlace(
                        const A1& a1, const A2& a2, const A3& a3, const A4& a4,
                        const A5& a5, const A6& a6, const A7& a7, const A8& a8,
                        const A9& a9, const A10& a10)
{
    typedef bsls::ObjectBuffer<TYPE> BufferType;

    reset();
    bslalg::ScalarPrimitives::construct(
                  &(reinterpret_cast<BufferType *>(&this->d_value)->object()),
                  a1, a2, a3, a4, a5, a6, a7, a8, a9, a10,
                  this->getAllocator());
    this->d_type = Variant_TypeIndex<TYPES, TYPE>::k_VaL;
}

template <class TYPES>
template <class TYPE, class A1, class A2, class A3, class A4, class A5,
                      class A6, class A7, class A8, class A9, class A10,
                      class A11>
inline
void VariantImp<TYPES>::createInPlace(
                        const A1& a1, const A2& a2, const A3& a3, const A4& a4,
                        const A5& a5, const A6& a6, const A7& a7, const A8& a8,
                        const A9& a9, const A10& a10, const A11& a11)
{
    typedef bsls::ObjectBuffer<TYPE> BufferType;

    reset();
    bslalg::ScalarPrimitives::construct(
                  &(reinterpret_cast<BufferType *>(&this->d_value)->object()),
                  a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11,
                  this->getAllocator());
    this->d_type = Variant_TypeIndex<TYPES, TYPE>::k_VaL;
}

template <class TYPES>
template <class TYPE, class A1, class A2, class A3, class A4, class A5,
                      class A6, class A7, class A8, class A9, class A10,
                      class A11, class A12>
inline
void VariantImp<TYPES>::createInPlace(
                        const A1& a1, const A2& a2, const A3& a3, const A4& a4,
                        const A5& a5, const A6& a6, const A7& a7, const A8& a8,
                        const A9& a9, const A10& a10, const A11& a11,
                        const A12& a12)
{
    typedef bsls::ObjectBuffer<TYPE> BufferType;

    reset();
    bslalg::ScalarPrimitives::construct(
                  &(reinterpret_cast<BufferType *>(&this->d_value)->object()),
                  a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12,
                  this->getAllocator());
    this->d_type = Variant_TypeIndex<TYPES, TYPE>::k_VaL;
}

template <class TYPES>
template <class TYPE, class A1, class A2, class A3, class A4, class A5,
                      class A6, class A7, class A8, class A9, class A10,
                      class A11, class A12, class A13>
inline
void VariantImp<TYPES>::createInPlace(
                        const A1& a1, const A2& a2, const A3& a3, const A4& a4,
                        const A5& a5, const A6& a6, const A7& a7, const A8& a8,
                        const A9& a9, const A10& a10, const A11& a11,
                        const A12& a12, const A13& a13)
{
    typedef bsls::ObjectBuffer<TYPE> BufferType;

    reset();
    bslalg::ScalarPrimitives::construct(
                  &(reinterpret_cast<BufferType *>(&this->d_value)->object()),
                  a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12,
                  a13, this->getAllocator());
    this->d_type = Variant_TypeIndex<TYPES, TYPE>::k_VaL;
}

template <class TYPES>
template <class TYPE, class A1, class A2, class A3, class A4, class A5,
                      class A6, class A7, class A8, class A9, class A10,
                      class A11, class A12, class A13, class A14>
inline
void VariantImp<TYPES>::createInPlace(
                        const A1& a1, const A2& a2, const A3& a3, const A4& a4,
                        const A5& a5, const A6& a6, const A7& a7, const A8& a8,
                        const A9& a9, const A10& a10, const A11& a11,
                        const A12& a12, const A13& a13, const A14& a14)
{
    typedef bsls::ObjectBuffer<TYPE> BufferType;

    reset();
    bslalg::ScalarPrimitives::construct(
                  &(reinterpret_cast<BufferType *>(&this->d_value)->object()),
                  a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12,
                  a13, a14, this->getAllocator());
    this->d_type = Variant_TypeIndex<TYPES, TYPE>::k_VaL;
}

template <class TYPES>
void VariantImp<TYPES>::reset()
{
    if (this->d_type) {
        Variant_DestructorVisitor destructor;
        apply(destructor);
        this->d_type = 0;
    }
}

template <class TYPES>
void VariantImp<TYPES>::swap(VariantImp<TYPES>& other)
{
    if (!this->d_type) {
        if (!other.d_type) {
            return;                                                   // RETURN
        }
        *this = other;
        other.reset();
    }
    else if (!other.d_type) {
        other = *this;
        this->reset();
    }
    else {
        if (this->d_type         == other.d_type
         && this->getAllocator() == other.getAllocator()) {
            // Same types and same allocators, so use a visitor that calls
            // 'swap'.

            Variant_SwapVisitor swapper(&this->d_value);
            other.apply(swapper);
        }
        else {
            // Different types and/or allocators, so swap via assignment.

            bsl::swap(*this, other);
        }
    }
}

template <class TYPES>
template <class TYPE>
inline
TYPE& VariantImp<TYPES>::the()
{
    BSLMF_ASSERT((Variant_TypeIndex<TYPES, TYPE>::k_VaL));
    BSLS_ASSERT_SAFE((this->d_type ==
                                 Variant_TypeIndex<TYPES, TYPE>::k_VaL));
    return reinterpret_cast<bsls::ObjectBuffer<TYPE> *>(
                                                     &this->d_value)->object();
}

// ACCESSORS
template <class TYPES>
inline
int VariantImp<TYPES>::maxSupportedBdexVersion() const
{
    if (this->d_type) {
        Variant_MaxSupportedBdexVersionVisitor visitor;
        doApply<Variant_MaxSupportedBdexVersionVisitor&>(visitor,
                                                           this->d_type);
        return visitor.d_maxSupportedBdexVersion;
    }

    return bdex_VersionFunctions::BDEX_NO_VERSION_NUMBER;
}

template <class TYPES>
template <class RET_TYPE, class VISITOR>
inline
RET_TYPE VariantImp<TYPES>::apply(VISITOR& visitor) const
{
    if (this->d_type) {
        return doApplyR<VISITOR&, RET_TYPE>(visitor, this->d_type);
    }

    bslmf::Nil nil = bslmf::Nil();
    return visitor(nil);
}

template <class TYPES>
template <class RET_TYPE, class VISITOR>
inline
RET_TYPE VariantImp<TYPES>::apply(const VISITOR& visitor) const
{
    if (this->d_type) {
        return doApplyR<const VISITOR&, RET_TYPE>(visitor, this->d_type);
    }

    bslmf::Nil nil = bslmf::Nil();
    return visitor(nil);
}

template <class TYPES>
template <class RET_TYPE, class VISITOR, class TYPE>
inline
RET_TYPE VariantImp<TYPES>::apply(VISITOR&    visitor,
                                        const TYPE& defaultValue) const
{
    if (this->d_type) {
        return doApplyR<VISITOR&, RET_TYPE>(visitor, this->d_type);
    }

    return visitor(defaultValue);
}

template <class TYPES>
template <class RET_TYPE, class VISITOR, class TYPE>
inline
RET_TYPE VariantImp<TYPES>::apply(const VISITOR& visitor,
                                        const TYPE&    defaultValue) const
{
    if (this->d_type) {
        return doApplyR<const VISITOR&, RET_TYPE>(visitor, this->d_type);
    }

    return visitor(defaultValue);
}

template <class TYPES>
template <class RET_TYPE, class VISITOR>
inline
RET_TYPE VariantImp<TYPES>::applyRaw(VISITOR& visitor) const
{

    typedef Variant_RawVisitorHelper<
                               RET_TYPE,
                               VISITOR> Helper;

    return doApplyR<const Helper&, RET_TYPE>(Helper(&visitor),
                                             this->d_type);
}

template <class TYPES>
template <class RET_TYPE, class VISITOR>
inline
RET_TYPE VariantImp<TYPES>::applyRaw(const VISITOR& visitor) const
{
    typedef Variant_RawVisitorHelper<
                               RET_TYPE,
                               const VISITOR> Helper;

    return doApplyR<const Helper&, RET_TYPE>(Helper(&visitor),
                                             this->d_type);

}

template <class TYPES>
template <class STREAM>
STREAM& VariantImp<TYPES>::bdexStreamOut(STREAM& stream,
                                               int     version) const
{
    bdex_OutStreamFunctions::streamOut(stream, this->d_type, 0);

    if (this->d_type) {
        typedef Variant_BdexStreamOutVisitor<STREAM> Streamer;

        Streamer streamer(stream, version);
        doApply<Streamer&>(streamer, this->d_type);
    }
    return stream;
}

template <class TYPES>
template <class TYPE>
inline
bool VariantImp<TYPES>::is() const
{
    return Variant_TypeIndex<TYPES, TYPE>::k_VaL == this->d_type;
}

template <class TYPES>
inline
bool VariantImp<TYPES>::isUnset() const
{
    return !this->d_type;
}

template <class TYPES>
bsl::ostream&
VariantImp<TYPES>::print(bsl::ostream& stream,
                               int           level,
                               int           spacesPerLevel) const
{
    if (this->d_type) {
        Variant_PrintVisitor visitor(&stream,
                                           level,
                                           spacesPerLevel);

        doApply<const Variant_PrintVisitor&>(visitor, this->d_type);
    }
    return stream;
}

template <class TYPES>
template <class TYPE>
inline
const TYPE& VariantImp<TYPES>::the() const
{
    BSLMF_ASSERT((Variant_TypeIndex<TYPES, TYPE>::k_VaL));
    BSLS_ASSERT_SAFE((this->d_type ==
                                 Variant_TypeIndex<TYPES, TYPE>::k_VaL));

    return reinterpret_cast<
                   const bsls::ObjectBuffer<TYPE> *>(&this->d_value)->object();
}

template <class TYPES>
inline
int VariantImp<TYPES>::typeIndex() const
{
    return this->d_type;
}

template <class TYPES>
inline
const bsl::type_info& VariantImp<TYPES>::typeInfo() const
{
    return typeid(void);
}
}  // close package namespace

// FREE OPERATORS
template <class TYPES>
bool bdlb::operator==(const VariantImp<TYPES>& lhs,
                const VariantImp<TYPES>& rhs)
{
    if (lhs.typeIndex() != rhs.typeIndex()) {
        return false;
    }

    if (0 == lhs.typeIndex()) {
        return true;
    }

    Variant_EqualityTestVisitor visitor(&rhs.d_value);
    lhs.apply(visitor);

    return visitor.d_result;
}

template <class TYPES>
inline
bool bdlb::operator!=(const VariantImp<TYPES>& lhs,
                const VariantImp<TYPES>& rhs)
{
    return !(lhs == rhs);
}

template <class TYPES>
inline
bsl::ostream& bdlb::operator<<(bsl::ostream&                  stream,
                         const VariantImp<TYPES>& rhs)
{
    return rhs.print(stream, 0, -1);
}

// FREE FUNCTIONS
template <typename TYPES>
inline
void swap(bdlb::VariantImp<TYPES>& a, bdlb::VariantImp<TYPES>& b)
{
    a.swap(b);
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
