//
//  AmalgamateTests.m
//  AmalgamateTests
//
//  Created by Jake Petroules on 2014-05-18.
//
//

#import <XCTest/XCTest.h>
#include "amg.h"

@interface AmalgamateTests : XCTestCase

@end

@implementation AmalgamateTests

- (void)setUp
{
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown
{
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}

- (void)testReadableFiles
{
    NSArray *paths = [[NSBundle bundleForClass:self.class] pathsForResourcesOfType:@"DS_Store" inDirectory:nil];
    XCTAssertTrue(paths.count > 0);
    for (NSString *path in paths) {
        dsstore_header_t header;
        int status = amg_dump_file(path.fileSystemRepresentation);
        XCTAssertEqual(status, 0);
    }
}

@end
