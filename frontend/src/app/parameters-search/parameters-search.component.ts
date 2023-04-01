import { Component, Input } from '@angular/core';
import { ParameterService } from '../services/parameter.service';
import {
  sanitizeType,
  sanitizeVar,
  searchScore,
  sortList,
  validateType,
  validateVar,
} from '../utils/utils';
import { ParameterGroup, Parameters, StringDict } from '../utils/interfaces';
import { ColComponent, Column, ColWidth } from '../search/search.component';
import { Pipe, PipeTransform } from '@angular/core';

export namespace ParameterGroupDrag {
  export function onDropOnGroup(event: DragEvent, row: ParameterGroup) {
    event.preventDefault();
    if (event.dataTransfer) {
      let data: string[] = event.dataTransfer.getData('text/plain').split(' ');
      let name: string = data[1];
      if (name) {
        row.addParam(name);
      }
    }
  }

  export function onDropOnSet(event: DragEvent, row: Parameters) {
    event.preventDefault();
    if (event.dataTransfer) {
      let data: string[] = event.dataTransfer.getData('text/plain').split(' ');
      let uuid: string = data[0];
      let name: string = data[1];
      if (name) {
        row.addParam(name);
      } else {
        row.addGroup(uuid);
      }
    }
  }

  export function onDragOver(event: DragEvent) {
    event.stopPropagation();
    event.preventDefault();
  }
}

@Pipe({
  name: 'parametergroup',
  pure: false,
})
export class ParameterGroupPipe implements PipeTransform {
  transform(
    items: Set<string>,
    parameterService: ParameterService
  ): ParameterGroup[] {
    return [...items].reduce((arr: ParameterGroup[], id: string) => {
      let group: ParameterGroup | undefined =
        parameterService.getParamGroup(id);
      if (group && group.params.size > 0) {
        arr.push(group);
      }
      return arr;
    }, []);
  }
}

@Component({
  selector: 'type-col-header',
  template: `<col-header
    [value]="value"
    [classes]="classes"
    (drop)="onDrop($event, row)"
    (dragover)="onDragOver($event)"
  ></col-header>`,
})
export class TypeColHeaderComponent implements ColComponent {
  @Input() row: Parameters = new Parameters();
  @Input() value: string = '';

  classes: string[] = [
    'type',
    'border-end-0',
    'rounded-0',
    'rounded-start',
    'text-end',
  ];

  onDrop = ParameterGroupDrag.onDropOnSet;
  onDragOver = ParameterGroupDrag.onDragOver;
}

@Component({
  selector: 'type-col-header',
  template: `<col-header
    [value]="value"
    [classes]="classes"
    (drop)="onDrop($event, row)"
    (dragover)="onDragOver($event)"
  ></col-header>`,
})
export class NameColHeaderComponent implements ColComponent {
  @Input() row: Parameters = new Parameters();
  @Input() value: string = '';

  classes: string[] = ['border-start-0', 'rounded-0', 'rounded-end'];

  onDrop = ParameterGroupDrag.onDropOnSet;
  onDragOver = ParameterGroupDrag.onDragOver;
}

@Component({
  selector: 'app-group',
  templateUrl: './group-display.component.html',
  styleUrls: ['./group-display.component.css'],
})
export class GroupComponent implements ColComponent {
  @Input() row: Parameters = new Parameters();
  @Input() value: Set<string> = new Set<string>();

  constructor(protected parameterService: ParameterService) {}

  onDrop = ParameterGroupDrag.onDropOnSet;
  onDragOver = ParameterGroupDrag.onDragOver;
}

// TODO: delete type-modal
@Component({
  selector: 'app-parameters-search',
  templateUrl: './parameters-search.component.html',
  styleUrls: ['./parameters-search.component.css'],
})
export class ParametersSearchComponent {
  cols: Column[] = [
    new Column({
      key: 'type',
      width: ColWidth.Fit,
      requireInput: true,
      inputPlaceholder: 'New: Type',
      validateInput: validateType,
      sanitizeInput: sanitizeType,
      component: TypeColHeaderComponent,
    }),
    new Column({
      key: 'name',
      width: ColWidth.Fit,
      requireInput: true,
      inputPlaceholder: 'Name',
      validateInput: (s: string): boolean =>
        validateVar(s) &&
        this.parameterService.paramSets.findIndex(
          (set: Parameters) => set.name === s
        ) === -1,
      sanitizeInput: sanitizeVar,
      component: NameColHeaderComponent,
    }),
    new Column({
      key: 'groups',
      component: GroupComponent,
    }),
  ];

  constructor(protected parameterService: ParameterService) {}

  sortParamSets(rows: Parameters[], query: string) {
    sortList(
      rows,
      searchScore(query, (set: Parameters) => [set.name, set.type])
    );
  }

  newSet(args: StringDict<string>) {
    this.parameterService.newParamSet(
      new Parameters({
        type: args['type'],
        name: args['name'],
      })
    );
  }
}
