//
//  IJContext.h
//  Injective
//
//  Created by Vladimir Pouzanov on 1/21/12.
//
//  Copyright (c) 2012 Vladimir Pouzanov.
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to 
//  deal in the Software without restriction, including without limitation the
//  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or 
//  sell copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
//  IN THE SOFTWARE.

#import <Foundation/Foundation.h>

#pragma mark Informal protocol for implementing

/** InjectiveProtocol defines helper methods that guide Injective in class instatiation
 */
@interface NSObject (InjectiveProtocol)

/** An array of properties, that must be set for object to come alive
 *
 *  The properties are filled in from Injective registered classes
 */
+ (NSSet *)injective_requiredProperties;

@end

#pragma mark - Helper methods on NSObject

@interface NSObject (Injective)

/** Create a new instance of the receiving class or get a singleton
 */
+ (id)injectiveInstantiateWithProperties:(id)firstValue, ...;
+ (id)injectiveInstantiateWithPropertiesDictionary:(NSDictionary *)properties;
+ (id)injectiveInstantiate;

@end

#pragma mark - IJContext

/** IJContextInstantiationMode defines the instatiation mode.
 *
 *  IJContextInstantiationModeFactory   -- create a new instance of class when requested
 *  IJContextInstantiationModeSingleton -- create an instance when first requested, then
 *                                          return it. IJContext -retain's all singleton
 *                                          objects.
 */
typedef enum IJContextInstantiationMode {
	IJContextInstantiationModeFactory,
	IJContextInstantiationModeSingleton
} IJContextInstantiationMode;

/** IJContextInstantiationBlock is used in custom initialization cases.
 *
 *  If the block of this type is passed to -registerClass:instantiationMode:instantiationBlock:,
 *  then it's used instead of [[[klass alloc] init] autorelease]. It must return a new instance
 *  of registering class. You can use the _props_ dictionary for local context.
 */
typedef id(^IJContextInstantiationBlock)(NSDictionary *props);

@interface IJContext : NSObject

+ (IJContext *)defaultContext;
+ (void)setDefaultContext:(IJContext *)context;

/// Generic class registration methods
- (void)registerClass:(Class)klass instantiationMode:(IJContextInstantiationMode)mode;
- (void)registerClass:(Class)klass instantiationMode:(IJContextInstantiationMode)mode instantiationBlock:(IJContextInstantiationBlock)block;

/// Singleton registration from existing instance
- (void)registerSingletonInstance:(id)obj forClass:(Class)klass;

/// Generic instantiation interface
- (id)instantiateClass:(Class)klass withProperties:(NSDictionary *)props;

/// Protocol instantiation interfaces
- (id)instantiateClassImplementingProtocol:(Protocol *)proto withProperties:(NSDictionary *)props;
- (NSArray *)instantiateAllClassesImplementingProtocol:(Protocol *)proto withProperties:(NSDictionary *)props;

@end

#pragma mark - Helper macros

/** injective_register(klass) adds the +initialize method that registers passed class in default context
 */
#define injective_register(klass) \
	+ (void)initialize	\
	{ \
		if(self == [klass class]) { \
			[[IJContext defaultContext] registerClass:[klass class] instantiationMode:IJContextInstantiationModeFactory]; \
		} \
	}

/** injective_register_singleton(klass) adds the +initialize method that registers passed class as a singleton in default context
 */
#define injective_register_singleton(klass) \
	+ (void)initialize	\
	{ \
		if(self == [klass class]) { \
			[[IJContext defaultContext] registerClass:[klass class] instantiationMode:IJContextInstantiationModeSingleton]; \
		} \
	}

/** injective_requires adds the +injective_requiredProperties method for IJContext
 */
#define injective_requires(...) \
	+ (NSSet *)injective_requiredProperties \
	{ \
		return [NSSet setWithObjects:__VA_ARGS__, nil]; \
	}