const std = @import("std");

// fixed

const FIGHTING_AGE_MAX = 50;
const FIGHTING_AGE_MIN = 20;
const WIFE_MIN_YEARS_SINCE_CHILD = 2;
const MALE_ODDS = 0.5;

// change

const START_COUNT = 50;

const MAX_WIVES = 10;
const ANNUAL_ADD_WIFE_ODDS = 0.002; // and concubines

const ANNUAL_CHILD_BIRTH_ODDS = 0.5;
const BIRTH_DEATH_ODDS = 0.1;
const ANNUAL_CHILD_DEATH_ODDS = 0.005;
const ANNUAL_ADULT_DEATH_ODDS = 0.002;

// precomputed box-muller approximation

const PRECOMPUTED_MARRIED_AGE_NORMAL = [23]u8{ // mean: 25; stdev: 5
    20, 21, 22, 22,
    23, 23, 24, 24, 24,
    25, 25, 25, 25, 25,
    26, 26, 26, 27, 27,
    28, 28, 29, 30
};

const PRECOMPUTED_START_AGE_NORMAL = [43]u8{ // mean: 25; stdev: 2
    5, 7, 8, 9, 10, 10, 11, 11,
    12, 12, 12, 13, 13, 13, 13,
    14, 14, 14, 14,
    15, 15, 15, 15, 15,
    16, 16, 16, 16,
    17, 17, 17, 17, 18, 18, 18,
    19, 19, 20, 20, 21, 22, 23, 25
};


const Hebrew = struct {
    age: u8 = 0,
    married_age: u8 = 0,
    wives: std.ArrayList(Wife) = std.ArrayList(Wife).init(std.heap.page_allocator),
};

const Wife = struct { // could be concubine, but for simplicity
    years_since_child: u8 = WIFE_MIN_YEARS_SINCE_CHILD,
};


pub fn main() !void {
    var timer = try std.time.Timer.start();

    // generate ages of starting sons (1728)

    var eligible_men_or_younger = std.ArrayList(Hebrew).init(std.heap.page_allocator);
    
    for (0..START_COUNT) |index| {
        const age = PRECOMPUTED_START_AGE_NORMAL[index % PRECOMPUTED_START_AGE_NORMAL.len];
        const married_age = PRECOMPUTED_MARRIED_AGE_NORMAL[index % PRECOMPUTED_MARRIED_AGE_NORMAL.len];

        var hebrew = Hebrew{ .age = age, .married_age = married_age };
        if (hebrew.age >= hebrew.married_age) {
            try hebrew.wives.append(Wife{});
        }
        try eligible_men_or_younger.append(hebrew);
    }

    // run sim to 1513 BCE (=215)
    var prng = std.rand.DefaultPrng.init(blk: {
        var seed: u64 = undefined;
        try std.posix.getrandom(std.mem.asBytes(&seed));
        break :blk seed;
    });   
    const rand = prng.random();
    for (0..215) |_| {
        // std.debug.print("\nyear: {}\n", .{1728-i-1});

        var new_list = std.ArrayList(Hebrew).init(std.heap.page_allocator);
        
        var index: usize = 0;
        for (eligible_men_or_younger.items) |*tmp_hebrew| {

            var hebrew = tmp_hebrew.*;
            hebrew.age += 1;

            if (hebrew.age > FIGHTING_AGE_MAX) continue;

            // marriage
            if (hebrew.age == hebrew.married_age) {
                try hebrew.wives.append(Wife{});
            } else if (rand.float(f32) <= ANNUAL_ADD_WIFE_ODDS) {
                try hebrew.wives.append(Wife{});
            }

            // wife and birth
            for (hebrew.wives.items) |*wife| {
                wife.years_since_child += 1;
                if (wife.years_since_child < WIFE_MIN_YEARS_SINCE_CHILD) continue;
                if (
                    rand.float(f32) <= ANNUAL_CHILD_BIRTH_ODDS and
                    rand.float(f32) <= MALE_ODDS and
                    rand.float(f32) > BIRTH_DEATH_ODDS
                ) {
                    //std.debug.print("birth\n", .{});
                    wife.years_since_child = 0;
                    const married_age = PRECOMPUTED_MARRIED_AGE_NORMAL[index % PRECOMPUTED_MARRIED_AGE_NORMAL.len];
                    try new_list.append(Hebrew{ .married_age = married_age });
                    index += 1;
                }
            }

            // child death
            if (hebrew.age < 18 and rand.float(f32) <= ANNUAL_CHILD_DEATH_ODDS) {
                continue;
            } else if (rand.float(f32) <= ANNUAL_ADULT_DEATH_ODDS) {
                continue;
            }

            try new_list.append(hebrew);
        }

        eligible_men_or_younger.items = new_list.items;
        //std.debug.print("count: {} \n", .{ eligible_men_or_younger.items.len });
    }
    std.debug.print("{} \n", .{ eligible_men_or_younger.items.len });

    const elapsed_sec = @as(f64, @floatFromInt(@divTrunc(timer.read(), 10_000_000))) / 100;
    std.debug.print("finished in: {d:2.}s \n", .{ elapsed_sec });
}
