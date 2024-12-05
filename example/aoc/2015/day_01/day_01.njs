import { part_1 } from "./part_1.njs"
import { part_2 } from "./part_2.njs"
import { part_err } from "../main.njs"

function day_01(part: number): (string): number {
    return part == 1 ? part_1 : part == 2 ? part_2 : part_err
}
