import { Pipe, PipeTransform } from '@angular/core';

// Sort pipe
@Pipe({
  name: 'sorted',
  pure: false,
})
export class SortPipe implements PipeTransform {
  transform(items: any): any {
    return [...items].sort();
  }
}
