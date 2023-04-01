import { Component, Injectable, Input } from '@angular/core';
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

@Injectable({
  providedIn: 'root',
})
export class ParameterDragService {
  readonly format: string = 'applicaton/json';

  constructor(private parameterService: ParameterService) {}

  onDragStart(event: DragEvent, data: ParameterDragService.DragData) {
    event.dataTransfer?.setData(this.format, JSON.stringify(data));
  }

  dropInTrash(event: DragEvent) {
    event.preventDefault();
    if (event.dataTransfer) {
      let data: ParameterDragService.DragData = JSON.parse(
        event.dataTransfer.getData(this.format)
      );
      if (data.type === ParameterDragService.DataType.Group) {
        this.parameterService.removeParamGroup(data.value);
      } else if (
        data.type === ParameterDragService.DataType.Var &&
        data.srcUUID
      ) {
        this.parameterService
          .getParamGroup(data.srcUUID)
          ?.removeParam(data.value);
      }
    }
  }

  dropOnGroup(event: DragEvent, group: ParameterGroup) {
    event.preventDefault();
    if (event.dataTransfer) {
      let data: ParameterDragService.DragData = JSON.parse(
        event.dataTransfer.getData(this.format)
      );
      if (data.type === ParameterDragService.DataType.Var) {
        group.addParam(data.value);
      }
    }
  }

  dropOnSet(event: DragEvent, set: Parameters) {
    event.preventDefault();
    if (event.dataTransfer) {
      let data: ParameterDragService.DragData = JSON.parse(
        event.dataTransfer.getData(this.format)
      );
      if (data.type === ParameterDragService.DataType.Group) {
        set.addGroup(data.value);
      } else if (data.type === ParameterDragService.DataType.Var) {
        set.addParam(data.value);
      }
    }
  }

  onDragOver(event: DragEvent) {
    event.stopPropagation();
    event.preventDefault();
  }
}

export namespace ParameterDragService {
  export enum DataType {
    Var = 0,
    Group = 1,
    Set = 2,
    Callback = 3,
  }

  export interface DragData {
    type: DataType;
    value: string;
    srcUUID?: string;
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
    (drop)="parameterDragService.dropOnSet($event, row)"
    (dragover)="parameterDragService.onDragOver($event)"
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

  constructor(protected parameterDragService: ParameterDragService) {}
}

@Component({
  selector: 'type-col-header',
  template: `<col-header
    [value]="value"
    [classes]="classes"
    (drop)="parameterDragService.dropOnSet($event, row)"
    (dragover)="parameterDragService.onDragOver($event)"
  ></col-header>`,
})
export class NameColHeaderComponent implements ColComponent {
  @Input() row: Parameters = new Parameters();
  @Input() value: string = '';

  classes: string[] = ['border-start-0', 'rounded-0', 'rounded-end'];

  constructor(protected parameterDragService: ParameterDragService) {}
}

@Component({
  selector: 'app-group',
  templateUrl: './group-display.component.html',
  styleUrls: ['./group-display.component.css'],
})
export class GroupComponent implements ColComponent {
  @Input() row: Parameters = new Parameters();
  @Input() value: Set<string> = new Set<string>();

  constructor(
    protected parameterService: ParameterService,
    protected parameterDragService: ParameterDragService
  ) {}
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
